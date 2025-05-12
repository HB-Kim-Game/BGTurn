// Fill out your copyright notice in the Description page of Project Settings.


#include "Arrow.h"

#include "BGUtil.h"
#include "CharacterActionData.h"
#include "DamageUI.h"
#include "DiceChecker.h"
#include "MouseControlledPlayer.h"
#include "MoveCharacterBase.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerUI.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AArrow::AArrow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetBoxExtent(FVector(35.f, 5.f, 8.f));
	SetRootComponent(BoxComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(BoxComponent);
	StaticMeshComponent->SetRelativeRotation(FRotator(90.0f, 0, 0.0f));

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh (TEXT("/Script/Engine.StaticMesh'/Game/BG3/Model/Rogue/arrow.arrow'"));

	if (tempMesh.Succeeded())
	{
		StaticMeshComponent->SetStaticMesh(tempMesh.Object);
	}
	
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> ns (TEXT("/Script/Niagara.NiagaraSystem'/Game/sA_Megapack_v1/sA_Projectilevfx/Vfx/Fx/Niagara_Systems/NS_Hit3.NS_Hit3'"));

	if (ns.Succeeded())
	{
		HitSystem = ns.Object;
	}
}

// Called when the game starts or when spawned
void AArrow::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AArrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bIsInitialized) return;

	FHitResult hit;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	params.AddIgnoredActor(ArrowInstigator);
	
	if (GetWorld()->SweepSingleByChannel(hit, GetActorLocation(), GetActorLocation(), FQuat::Identity, ECC_WorldDynamic, FCollisionShape::MakeBox(FVector(35.f, 5.f, 8.f)), params))
	{
		if (auto* cast = Cast<AMoveCharacterBase>(hit.GetActor()))
		{
			if (ActionData->Target != cast) return;
			uint8 statBonus = ArrowInstigator->Status.Dex;
	
			int8 diceResult = UDiceChecker::RollDice();
		
			// 대성공
			if (diceResult >= 20)
			{
				// 피해 굴림
				// 원거리 기본 피해량(1d8을 기준으로 함.) + 민첩 능력치 수정값
				// 대성공일 경우, dice 두번 굴림.
				UE_LOG(LogTemp, Warning, TEXT("Critical"));
				int8 damageResult = UDiceChecker::RollDice(8) + UDiceChecker::RollDice(8) + UBGUtil::CalculateBonus(statBonus) * 2;
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
				
				UGameplayStatics::ApplyDamage(cast, damageResult, ArrowInstigator->GetController(), this, UDamageType::StaticClass());
				SpawnHitEffect(hit.ImpactPoint);
				return;
			}

					
			// 무조건 무기에 숙련되어 있다는 기준, 숙련도 보너스 +2 를 함.
			// 공격 판정 성공
			// 고지대 보정
			int bonus = ArrowInstigator->GetActorLocation().Z >= hit.GetActor()->GetActorLocation().Z + 250.f ? 2 :
							ArrowInstigator->GetActorLocation().Z <= hit.GetActor()->GetActorLocation().Z - 250.f ? -2 : 0;

			// 원거리를 근거리로 공격할 경우 보정
			bonus += FVector::Distance(ArrowInstigator->GetActorLocation(), hit.GetActor()->GetActorLocation()) / 100.f < ActionData->MinDistance ? -2 : 0;
			
			// 무조건 무기에 숙련되어 있다는 기준, 숙련도 보너스 +2 를 함.
			// 공격 판정 성공
			if (diceResult + UBGUtil::CalculateBonus(statBonus) + 2 + bonus >= cast->Status.Defensive)
			{
				// 피해 굴림
				// 원거리 기본 피해량(1d8을 기준으로 함.) + 민첩 능력치 수정값
				// 대성공일 경우, dice 두번 굴림.
				UE_LOG(LogTemp, Warning, TEXT("Success"));
				int32 damageResult = UDiceChecker::RollDice(8) + UBGUtil::CalculateBonus(statBonus);
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
		
				UGameplayStatics::ApplyDamage(ActionData->Target, damageResult, ArrowInstigator->GetController(), this, UDamageType::StaticClass());
				SpawnHitEffect(hit.ImpactPoint);
				return;
			}
			
			FVector dir = ArrowInstigator->GetActorLocation() - cast->GetActorLocation();
			dir.Z = 0;
			FRotator rotation = dir.Rotation();
			cast->SetActorRotation(rotation);
			cast->PlayAnimation(TEXT("Dodge"));
			cast->GetDamageUI()->ShowDamage(cast->GetActorLocation(), -1);
			SpawnHitEffect(hit.ImpactPoint);
			return;
		}

		SpawnHitEffect(hit.ImpactPoint);
	}
}

void AArrow::Initialize(class UCharacterActionData* data, class AMoveCharacterBase* instigator)
{
	ActionData = data;
	ArrowInstigator = instigator;
	
	bIsInitialized = true;	
}

void AArrow::SpawnHitEffect(FVector location)
{
	if (HitSystem != nullptr)
	{
		UNiagaraComponent* hitEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitSystem, location);
		hitEffect->SetActive(true);
		Destroy();
	}
}

