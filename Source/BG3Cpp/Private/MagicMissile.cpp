// Fill out your copyright notice in the Description page of Project Settings.


#include "MagicMissile.h"

#include "BGUtil.h"
#include "CharacterActionData.h"
#include "DamageUI.h"
#include "DiceChecker.h"
#include "MouseControlledPlayer.h"
#include "MoveCharacterBase.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerUI.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

AMagicMissile::AMagicMissile()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(26.f);
	SetRootComponent(SphereComp);
	
	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	NiagaraComp->SetupAttachment(SphereComp);
	NiagaraComp->SetRelativeRotation(FRotator(0, -180.f, 0));

	ConstructorHelpers::FObjectFinder<UNiagaraSystem> ns (TEXT("/Script/Niagara.NiagaraSystem'/Game/sA_Megapack_v1/sA_Projectilevfx/Vfx/Fx/Niagara_Systems/NS_Projectile2_Small1.NS_Projectile2_Small1'"));

	ConstructorHelpers::FObjectFinder<UNiagaraSystem> ns2 (TEXT("/Script/Niagara.NiagaraSystem'/Game/sA_Megapack_v1/sA_Projectilevfx/Vfx/Fx/Niagara_Systems/NS_Hit4.NS_Hit4'"));
	if (ns.Succeeded())
	{
		NiagaraSystem = ns.Object;
	}

	if (ns2.Succeeded())
	{
		HitSystem = ns2.Object;
	}
}

void AMagicMissile::BeginPlay()
{
	Super::BeginPlay();

	if (NiagaraSystem != nullptr)
	{
		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem, SphereComp, TEXT(""), FVector::Zero(), FRotator(0, -180.f, 0), EAttachLocation::Type::KeepRelativeOffset, false, true);
		NiagaraComp->SetActive(true);
	}
	if (NiagaraComp)
	{
		NiagaraComp->Activate();
	}
}

void AMagicMissile::TickAction(float DeltaTime)
{
	Super::TickAction(DeltaTime);

	FVector BestDirection = FindBestDirection();

	FRotator TargetRotation = FMath::RInterpTo(GetActorRotation(), BestDirection.Rotation(), DeltaTime, TurnSpeed);
	SetActorRotation(TargetRotation);
	SetActorLocation(GetActorLocation() + GetActorForwardVector() * Speed * DeltaTime, true);

	FHitResult hit;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	params.AddIgnoredActor(AttackInstigator);
	
	if (GetWorld()->SweepSingleByChannel(hit, GetActorLocation(), GetActorLocation(), FQuat::Identity, ECC_WorldDynamic, FCollisionShape::MakeSphere(26.f), params))
	{
		if (auto* cast = Cast<AMoveCharacterBase>(hit.GetActor()))
		{
			if (nullptr == Target) return;
			if (cast != Target) return;
			uint8 statBonus = AttackInstigator->Status.Int;
	
			int8 diceResult = UDiceChecker::RollDice();

			// 대성공
			if (diceResult >= 20)
			{
				// 피해 굴림
				// 파이어볼 기본 피해량(1d4를 기준으로 함.) + 지능 능력치 수정값
				// 대성공일 경우, dice 두번 굴림.
				UE_LOG(LogTemp, Warning, TEXT("Critical"));
				int8 damageResult = UDiceChecker::RollDice(4) + UDiceChecker::RollDice(4) + UBGUtil::CalculateBonus(statBonus) * 2;
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
				
				UGameplayStatics::ApplyDamage(cast, damageResult, AttackInstigator->GetController(), this, UDamageType::StaticClass());
				SpawnHitEffect(hit.ImpactPoint);
				return;
			}
			
			// 공격 판정 성공

			// 피해 굴림
			// 무기 기본 피해량(1d4을 기준으로 함.) + 능력치 수정값
			UE_LOG(LogTemp, Warning, TEXT("Success"));
			int32 damageResult = UDiceChecker::RollDice(4) + UBGUtil::CalculateBonus(statBonus);
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
		
			UGameplayStatics::ApplyDamage(cast, damageResult, AttackInstigator->GetController(), this, UDamageType::StaticClass());
			SpawnHitEffect(hit.ImpactPoint);
		}
	}
}

FVector AMagicMissile::FindBestDirection()
{
	FVector CurrentLocation = GetActorLocation();
	FVector Forward = GetActorForwardVector();
	FVector Right = GetActorRightVector();
	FVector Up = FVector::UpVector;

	FVector TargetLocation = Target->GetActorLocation();
	FVector DesiredDirection = (TargetLocation - CurrentLocation).GetSafeNormal();

	// 상하좌우 충돌 판단용 방향
	TArray<FVector> CheckDirections =
		{
		Forward,
		Forward + Right,
		Forward - Right,
		Forward + Up,
		Forward - Up,
		Forward + Right + Up,
		Forward - Right + Up,
		Forward + Right - Up,
		Forward - Right - Up,
		};

	for (FVector& Dir : CheckDirections) Dir.Normalize();
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(AttackInstigator);

	FVector BestDirection = Forward;
	float bestScore = -1.f;
	bool allNotHit = true;

	for (const FVector& Dir : CheckDirections)
	{
		FVector End = CurrentLocation + Dir * TraceDistance;
		
		FHitResult Hit;
		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, CurrentLocation, End, ECC_EngineTraceChannel2, Params);
		
		if (!bHit)
		{
			// 충돌하지 않고, 가장 근접한 방향 체크
			float Score = FVector::DotProduct(Dir, DesiredDirection);
			if (Score > bestScore)
			{
				bestScore = Score;
				BestDirection = Dir;
			}	
		}
		else
		{
			if (Hit.GetActor()->GetClass() == StaticClass()) continue;
			allNotHit = false;
		}
	}

	if (allNotHit) BestDirection = DesiredDirection;

	return BestDirection;
}
