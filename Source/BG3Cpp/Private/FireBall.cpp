// Fill out your copyright notice in the Description page of Project Settings.


#include "FireBall.h"

#include "BGUtil.h"
#include "CharacterActionData.h"
#include "DamageUI.h"
#include "DiceChecker.h"
#include "MouseControlledPlayer.h"
#include "MoveCharacterBase.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerUI.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFireBall::AFireBall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetSphereRadius(26.f);
	SetRootComponent(SphereComponent);
	
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	NiagaraComponent->SetupAttachment(SphereComponent);
	NiagaraComponent->SetRelativeRotation(FRotator(0, -180.f, 0));

	ConstructorHelpers::FObjectFinder<UNiagaraSystem> ns (TEXT("/Script/Niagara.NiagaraSystem'/Game/sA_Megapack_v1/sA_Projectilevfx/Vfx/Fx/Niagara_Systems/NS_Projectile2_Small.NS_Projectile2_Small'"));

	ConstructorHelpers::FObjectFinder<UNiagaraSystem> ns2 (TEXT("/Script/Niagara.NiagaraSystem'/Game/sA_Megapack_v1/sA_Projectilevfx/Vfx/Fx/Niagara_Systems/NS_Hit2.NS_Hit2'"));
	if (ns.Succeeded())
	{
		NiagaraSystem = ns.Object;
	}

	if (ns2.Succeeded())
	{
		HitSystem = ns2.Object;
	}
}

// Called when the game starts or when spawned
void AFireBall::BeginPlay()
{
	Super::BeginPlay();

	if (NiagaraSystem != nullptr)
	{
		NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem, SphereComponent, TEXT(""), FVector::Zero(), FRotator::ZeroRotator, EAttachLocation::Type::KeepRelativeOffset, false, true);
		NiagaraComponent->SetActive(true);
	}
	if (NiagaraComponent)
	{
		NiagaraComponent->Activate();
	}
}

// Called every frame
void AFireBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsInitialized) return;

	FHitResult hit;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	params.AddIgnoredActor(FireballInstigator);
	
	if (GetWorld()->SweepSingleByChannel(hit, GetActorLocation(), GetActorLocation(), FQuat::Identity, ECC_WorldDynamic, FCollisionShape::MakeSphere(26.f), params))
	{
		if (auto* cast = Cast<AMoveCharacterBase>(hit.GetActor()))
		{
			if (ActionData->Target != cast) return;
			uint8 statBonus = FireballInstigator->Status.Int;
	
			int8 diceResult = UDiceChecker::RollDice();
		
			// 대성공
			if (diceResult >= 20)
			{
				// 피해 굴림
				// 파이어볼 기본 피해량(1d10을 기준으로 함.) + 지능 능력치 수정값
				// 대성공일 경우, dice 두번 굴림.
				UE_LOG(LogTemp, Warning, TEXT("Critical"));
				int8 damageResult = UDiceChecker::RollDice(10) + UDiceChecker::RollDice(10) + UBGUtil::CalculateBonus(statBonus) * 2;
				UE_LOG(LogTemp, Warning, TEXT("%d"), damageResult);
				
				cast->GetDamageUI()->ShowDamage(cast->GetActorLocation(), damageResult);
				
				ExecuteActionHandle = cast->OnTakeDefaultDamage.Add(FOnTakeDefaultDamage::FDelegate::CreateLambda([this, cast]
					(float Damage, AMoveCharacterBase* damagedCharacter, AMoveCharacterBase* instigator)
				{
					if (auto* p = Cast<AMouseControlledPlayer>(damagedCharacter->GetWorld()->GetFirstPlayerController()->GetPawn()))
					{
						p->GetPlayerUI()->ShowSelectedObjectInfo(damagedCharacter);
					}
					bool isSuccess = cast->OnTakeDefaultDamage.Remove(ExecuteActionHandle);
					UE_LOG(LogTemp, Warning, TEXT("%d"), isSuccess);
				}));
				
				UGameplayStatics::ApplyDamage(cast, damageResult, FireballInstigator->GetController(), this, UDamageType::StaticClass());
				SpawnHitEffect(hit.ImpactPoint);
				return;
			}
			
			// 무조건 무기에 숙련되어 있다는 기준, 숙련도 보너스 +2 를 함.
			// 공격 판정 성공
			if (diceResult + UBGUtil::CalculateBonus(statBonus) + 2 >= cast->Status.Defensive)
			{
				// 피해 굴림
				// 무기 기본 피해량(1d6을 기준으로 함.) + 능력치 수정값
				UE_LOG(LogTemp, Warning, TEXT("Success"));
				int32 damageResult = UDiceChecker::RollDice(10) + UBGUtil::CalculateBonus(statBonus);
				UE_LOG(LogTemp, Warning, TEXT("%d"), damageResult);
			
				cast->GetDamageUI()->ShowDamage(cast->GetActorLocation(), damageResult);
			
				ExecuteActionHandle = cast->OnTakeDefaultDamage.Add(FOnTakeDefaultDamage::FDelegate::CreateLambda([this, cast]
					(float Damage, AMoveCharacterBase* damagedCharacter, AMoveCharacterBase* instigator)
				{
					if (auto* p = Cast<AMouseControlledPlayer>(damagedCharacter->GetWorld()->GetFirstPlayerController()->GetPawn()))
					{
						p->GetPlayerUI()->ShowSelectedObjectInfo(damagedCharacter);
					}
					bool isSuccess = cast->OnTakeDefaultDamage.Remove(ExecuteActionHandle);
				}));
		
				UGameplayStatics::ApplyDamage(ActionData->Target, damageResult, FireballInstigator->GetController(), this, UDamageType::StaticClass());
				SpawnHitEffect(hit.ImpactPoint);
				return;
			}
			
			FVector dir = FireballInstigator->GetActorLocation() - cast->GetActorLocation();
			dir.Z = 0;
			FRotator rotation = dir.Rotation();
			cast->SetActorRotation(rotation);
			cast->PlayAnimation(TEXT("Dodge"));
			cast->GetDamageUI()->ShowDamage(cast->GetActorLocation(), -1);
			SpawnHitEffect(hit.ImpactPoint);
		}
	}
}

void AFireBall::Initialize(class UCharacterActionData* data, class AMoveCharacterBase* instigator)
{
	ActionData = data;
	FireballInstigator = instigator;
	
	bIsInitialized = true;	
}

void AFireBall::SpawnHitEffect(FVector hitLocation)
{
	if (HitSystem != nullptr)
	{
		UNiagaraComponent* hitEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitSystem, hitLocation);
		hitEffect->SetActive(true);
		Destroy();
	}
}

