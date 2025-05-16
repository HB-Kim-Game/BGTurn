// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionBase.h"
#include "ActionCursor.h"
#include "Arrow.h"
#include "AttackRange.h"
#include "CharacterActionData.h"
#include "DiceChecker.h"
#include "BGUtil.h"
#include "BG3Enums.h"
#include "CharacterStatus.h"
#include "CustomTimer.h"
#include "DamageUI.h"
#include "FireBall.h"
#include "JumpCursor.h"
#include "MagicMissile.h"
#include "MouseControlledPlayer.h"
#include "MouseManager.h"
#include "MoveCharacterBase.h"
#include "MultiTargetActionCursor.h"
#include "ParabolaSpline.h"
#include "PlayableCharacterBase.h"
#include "PlayerUI.h"
#include "Kismet/GameplayStatics.h"

void UActionBase::PrepareAction(AMoveCharacterBase* character, UCharacterActionData* action)
{
	TArray<AActor*> actors;
	character->GetAttachedActors(actors);

	for (auto* actor : actors)
	{
		if (auto* cast = Cast<AAttackRange>(actor))
		{
			cast->Destroy();
		}
		if (auto* cast = Cast<AParabolaSpline>(actor))
		{
			TArray<AActor*> parabolaChildren;
			cast->GetAttachedActors(parabolaChildren);
			for (auto* child : parabolaChildren)
			{
				child->Destroy();
			}
			cast->Destroy();
		}
	}
}

void UActionBase::ExecuteAction(AMoveCharacterBase* character, UCharacterActionData* action)
{
	TArray<AActor*> actors;
	character->GetAttachedActors(actors);

	for (auto* actor : actors)
	{
		if (auto* cast = Cast<AAttackRange>(actor))
		{
			cast->Destroy();
		}
		if (auto* cast = Cast<AParabolaSpline>(actor))
		{
			TArray<AActor*> parabolaChildren;
			cast->GetAttachedActors(parabolaChildren);
			for (auto* child : parabolaChildren)
			{
				child->Destroy();
			}
			cast->Destroy();
		}
	}
}

void UMeleeAction::PrepareAction(AMoveCharacterBase* character, UCharacterActionData* action)
{
	Super::PrepareAction(character, action);

	if (auto* playable = Cast<APlayableCharacterBase>(character))
	{
		playable->SetSplineCondition(true);
	
	}

	// 캐릭터 주변으로 범위 표시
	AAttackRange* decal = character->GetWorld()->SpawnActor<AAttackRange>(
		AAttackRange::StaticClass(),
		character->GetActorLocation(),
		FRotator(0, 0, 0));

	decal->SetDecalRange(action->MaxDistance);
	
	decal->AttachToActor(character, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));

	// 캐릭터 애니메이션 재생
	FString prepareID = action->ActionID + "_Prepare";
	character->PlayAnimation(prepareID);
	
	// 마우스 커서 교체

	if (auto* p = Cast<AMouseControlledPlayer>(character->GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		p->GetMouseManager()->SetMouseMode(EGameMouseState::Action);

		if (auto* cursor = Cast<UActionCursor>(p->GetMouseManager()->GetCursor()))
		{
			cursor->ShowActionDescription(action, 0);
		}
	}
}

void UMeleeAction::ExecuteAction(AMoveCharacterBase* character, UCharacterActionData* action)
{
	Super::ExecuteAction(character, action);

	// 캐릭터 애니메이션 재생
	FString prepareID = action->ActionID + "_Execute";
	character->PlayAnimation(prepareID);
	UE_LOG(LogTemp, Warning, TEXT("Melee"));
	
	if (action->Target.Num() <= 0) return;

	uint8 statBonus = character->Status.Str > character->Status.Dex ? character->Status.Str : character->Status.Dex;

	int8 diceResult = UDiceChecker::RollDice();

	// 대성공
	if (diceResult >= 20)
	{
		// 피해 굴림
		// 무기 기본 피해량(1d6을 기준으로 함.) + 능력치 수정값
		// 대성공일 경우, dice 두번 굴림.
		UE_LOG(LogTemp, Warning, TEXT("Critical"));
		int8 damageResult = UDiceChecker::RollDice(6) + UDiceChecker::RollDice(6) + UBGUtil::CalculateBonus(statBonus) * 2;
		UE_LOG(LogTemp, Warning, TEXT("%d"), damageResult);
		
		action->Target[0]->GetDamageUI()->ShowDamage(action->Target[0]->GetActorLocation(), damageResult);
		
		ExecuteActionHandle = action->Target[0]->OnTakeDefaultDamage.Add(FOnTakeDefaultDamage::FDelegate::CreateLambda([action, this]
			(float Damage, AMoveCharacterBase* damagedCharacter, AMoveCharacterBase* instigator)
		{
			if (auto* p = Cast<AMouseControlledPlayer>(damagedCharacter->GetWorld()->GetFirstPlayerController()->GetPawn()))
			{
				p->GetPlayerUI()->ShowSelectedObjectInfo(damagedCharacter);
			}
			bool isSuccess = action->Target[0]->OnTakeDefaultDamage.Remove(ExecuteActionHandle);
			UE_LOG(LogTemp, Warning, TEXT("%d"), isSuccess);
		}));
		
		UGameplayStatics::ApplyDamage(action->Target[0], damageResult, character->GetController(), character, UDamageType::StaticClass());	
		return;
	}

	// 무조건 무기에 숙련되어 있다는 기준, 숙련도 보너스 +2 를 함.
	// 공격 판정 성공
	if (diceResult + UBGUtil::CalculateBonus(statBonus) + 2 >= action->Target[0]->Status.Defensive)
	{
		// 피해 굴림
		// 무기 기본 피해량(1d6을 기준으로 함.) + 능력치 수정값
		UE_LOG(LogTemp, Warning, TEXT("Success"));
		int32 damageResult = FMath::Max(UDiceChecker::RollDice(6) + UBGUtil::CalculateBonus(statBonus), 0);
		UE_LOG(LogTemp, Warning, TEXT("%d"), damageResult);

		action->Target[0]->GetDamageUI()->ShowDamage(action->Target[0]->GetActorLocation(), damageResult);

		ExecuteActionHandle = action->Target[0]->OnTakeDefaultDamage.Add(FOnTakeDefaultDamage::FDelegate::CreateLambda([action, this]
			(float Damage, AMoveCharacterBase* damagedCharacter, AMoveCharacterBase* instigator)
		{
			if (auto* p = Cast<AMouseControlledPlayer>(damagedCharacter->GetWorld()->GetFirstPlayerController()->GetPawn()))
			{
				p->GetPlayerUI()->ShowSelectedObjectInfo(damagedCharacter);
			}
			bool isSuccess = action->Target[0]->OnTakeDefaultDamage.Remove(ExecuteActionHandle);
			UE_LOG(LogTemp, Warning, TEXT("%d"), isSuccess);
		}));
		
		UGameplayStatics::ApplyDamage(action->Target[0], damageResult, character->GetController(), character, UDamageType::StaticClass());
		return;
	}

	FVector dir = character->GetActorLocation() - action->Target[0]->GetActorLocation();
	dir.Z = 0;
	FRotator rotation = dir.Rotation();
	action->Target[0]->SetActorRotation(rotation);
	action->Target[0]->PlayAnimation(TEXT("Dodge"));
	action->Target[0]->GetDamageUI()->ShowDamage(action->Target[0]->GetActorLocation(), -1);
}

void USprintAction::PrepareAction(AMoveCharacterBase* character, UCharacterActionData* action)
{
	Super::PrepareAction(character, action);

	// 캐릭터 애니메이션 재생
	FString prepareID = action->ActionID + "_Prepare";
	character->PlayAnimation(prepareID);

	// 마우스 커서 교체

	if (auto* p = Cast<AMouseControlledPlayer>(character->GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		p->GetMouseManager()->SetMouseMode(EGameMouseState::Action);

		if (auto* cursor = Cast<UActionCursor>(p->GetMouseManager()->GetCursor()))
		{
			cursor->ShowActionDescription(action, 100);
		}
	}
}

void USprintAction::ExecuteAction(AMoveCharacterBase* character, UCharacterActionData* action)
{
	Super::ExecuteAction(character, action);
	
	// 캐릭터 애니메이션 재생
	FString prepareID = action->ActionID + "_Execute";
	character->PlayAnimation(prepareID);
	UE_LOG(LogTemp, Warning, TEXT("Sprint"));

	character->AddMOV(character->Status.MOV);
}

void UJumpAction::PrepareAction(AMoveCharacterBase* character, UCharacterActionData* action)
{
	if (character->GetCurrentMov() < 3.0f) return;
	
	Super::PrepareAction(character, action);
	
	// 캐릭터 애니메이션 재생
	FString prepareID = action->ActionID + "_Prepare";
	character->PlayAnimation(prepareID);

	if (auto* playable = Cast<APlayableCharacterBase>(character))
	{
		// 캐릭터 주변으로 범위 표시
		AAttackRange* decal = character->GetWorld()->SpawnActor<AAttackRange>(
			AAttackRange::StaticClass(),
			character->GetActorLocation(),
			FRotator(0, 0, 0));

		float jumpHeight = (action->MaxDistance + UBGUtil::CalculateBonus(character->Status.Str));
		decal->SetDecalRange(jumpHeight);
	
		decal->AttachToActor(character, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));

		// 도약 거리 공식
		// 도약은 기본 4.5m + 힘 스탯 8에서 2 증가할때마다 1m 증가

		AParabolaSpline* parabola = character->GetWorld()->SpawnActor<AParabolaSpline>(
			AParabolaSpline::StaticClass(),
			character->GetActorLocation(),
			FRotator(0,0,0));

		parabola->InitializeParabola(0.025f, 200.f);

		parabola->AttachToActor(character, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));

		playable->SetSplineCondition(false);

		if (auto* p = Cast<AMouseControlledPlayer>(character->GetWorld()->GetFirstPlayerController()->GetPawn()))
		{
			p->GetMouseManager()->SetMouseMode(EGameMouseState::Jump);
		
			if (auto* cursor = Cast<UJumpCursor>(p->GetMouseManager()->GetCursor()))
			{
				cursor->InitializeCursor(jumpHeight, action);
				cursor->ShowJumpDescription();
			}
		}	
	}
}

void UJumpAction::ExecuteAction(AMoveCharacterBase* character, UCharacterActionData* action)
{
	// 점프는 이동거리 3m를 소모함.
	character->AddMOV(-3.f, false);
	// 캐릭터 애니메이션 재생
	FString prepareID = action->ActionID + "_Execute";
	character->PlayAnimation(prepareID);

	if (auto* playable = Cast<APlayableCharacterBase>(character))
	{
		playable->SetSplineCondition(true);
		TArray<AActor*> actors;
		character->GetAttachedActors(actors);
		FVector startLocation = character->GetActorLocation();
		FVector destination = FVector::ZeroVector;
		float height = 0.0f;

		for (auto* actor : actors)
		{
			if (auto* cast = Cast<AParabolaSpline>(actor))
			{
				AParabolaSpline* spline = cast;
				destination = spline->GetDestination();
				height = spline->GetHeight();
			}
		}
		// 점프
		character->SetIsMovable(false);
		UCustomTimer::SetTimer(character->GetWorld(), 1.0f, [character, startLocation, destination, height](float alpha)
		{
			FVector dir = destination - character->GetActorLocation();
			dir.Z = 0;
			FRotator rotation = dir.Rotation();
			character->SetActorRotation(rotation);
			character->SetActorLocation(UBGUtil::CalculateParabola(startLocation, destination, height,  alpha), true);
		},
		[character, startLocation, destination]()
		{
			character->SetIsMovable(true);
			UE_LOG(LogTemp, Warning, TEXT("점프 완료"));

			// 낙하거리가 4m 보다 클 경우 데미지
			float fallingDistance = startLocation.Z - destination.Z;

			if (fallingDistance >= 400.f)
			{
				int32 damageResult = UBGUtil::CalculateFallingDamage(character->GetStatus()->GetHp(), fallingDistance);
				UGameplayStatics::ApplyDamage(character, damageResult, character->GetController(), character, UDamageType::StaticClass());
				character->GetDamageUI()->ShowDamage(character->GetActorLocation(), damageResult);
				
			}
		});
	}

	Super::ExecuteAction(character, action);
}

void UFireBallAction::PrepareAction(AMoveCharacterBase* character, UCharacterActionData* action)
{
	Super::PrepareAction(character, action);

	// 캐릭터 애니메이션 재생
	FString prepareID = action->ActionID + "_Prepare";
	character->PlayAnimation(prepareID);

	if (auto* playable = Cast<APlayableCharacterBase>(character))
	{
		// 캐릭터 주변으로 범위 표시
		AAttackRange* decal = character->GetWorld()->SpawnActor<AAttackRange>(
			AAttackRange::StaticClass(),
			character->GetActorLocation(),
			FRotator(0, 0, 0));

		decal->SetDecalRange(action->MaxDistance);
		playable->SetSplineCondition(false);
	
		decal->AttachToActor(character, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));

		FVector parabolaLocation = character->GetMesh()->GetSocketLocation(TEXT("StaffSocket"));

		AParabolaSpline* parabola = character->GetWorld()->SpawnActor<AParabolaSpline>(
		AParabolaSpline::StaticClass(),
		parabolaLocation,
		FRotator(0,0,0));

		parabola->InitializeParabola(1.0f, 0);

		parabola->AttachToActor(character, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
		
		// 마우스 커서 교체

		if (auto* p = Cast<AMouseControlledPlayer>(character->GetWorld()->GetFirstPlayerController()->GetPawn()))
		{
			p->GetMouseManager()->SetMouseMode(EGameMouseState::Action);

			if (auto* cursor = Cast<UActionCursor>(p->GetMouseManager()->GetCursor()))
			{
				cursor->ShowActionDescription(action, 0);
			}
		}
	}

}

void UFireBallAction::ExecuteAction(AMoveCharacterBase* character, UCharacterActionData* action)
{
	// 캐릭터 애니메이션 재생
	FString prepareID = action->ActionID + "_Execute";
	character->PlayAnimation(prepareID);

	AFireBall* fireBall = character->GetWorld()->SpawnActor<AFireBall>(
	AFireBall::StaticClass(),
	character->GetMesh()->GetSocketLocation(TEXT("StaffSocket")),
	FRotator(0, 0, 0));

	FVector startPosition = character->GetMesh()->GetSocketLocation(TEXT("StaffSocket"));
	AMoveCharacterBase* Target = action->Target.Num() > 0 ? action->Target[0] : nullptr;
	fireBall->Initialize(action, character, Target);
	
	if (auto* playable = Cast<APlayableCharacterBase>(character))
	{
		playable->SetSplineCondition(true);
		TArray<AActor*> actors;
		character->GetAttachedActors(actors);
		FVector destination = FVector::ZeroVector;
		float height = 0.f;

		for (auto* actor : actors)
		{
			if (auto* cast = Cast<AParabolaSpline>(actor))
			{
				AParabolaSpline* spline = cast;
				spline->SetbIsExecute(true);
				destination = spline->GetDestination();
				height = spline->GetHeight();
			}
		}
		
		// 파이어볼 이동
		UCustomTimer::SetTimer(character->GetWorld(), 0.3f, [destination, fireBall, startPosition, height](float alpha)
		{
			if (fireBall)
			{
				FVector dir = fireBall->GetActorLocation() - destination;
				FRotator rotation = dir.Rotation();
				fireBall->SetActorRotation(rotation);
				fireBall->SetActorLocation(UBGUtil::CalculateParabola(startPosition,destination, height, alpha), true);	
			}
		},
		[destination, fireBall, startPosition, height]()
		{
			if (fireBall)
			{
				fireBall->SetActorLocation(UBGUtil::CalculateParabola(startPosition,destination, height, 1.0f), true);
			}
		});
	}
	Super::ExecuteAction(character, action);
}

void URangedAttackAction::PrepareAction(class AMoveCharacterBase* character, class UCharacterActionData* action)
{
	Super::PrepareAction(character, action);

	// 캐릭터 애니메이션 재생
	FString prepareID = action->ActionID + "_Prepare";
	character->PlayAnimation(prepareID);
	
	if (auto* playable = Cast<APlayableCharacterBase>(character))
	{
		// 캐릭터 주변으로 범위 표시
		AAttackRange* decal = character->GetWorld()->SpawnActor<AAttackRange>(
			AAttackRange::StaticClass(),
			character->GetActorLocation(),
			FRotator(0, 0, 0));

		decal->SetDecalRange(action->MaxDistance);
		playable->SetSplineCondition(false);
	
		decal->AttachToActor(character, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));

		UCustomTimer::SetTimer(character->GetWorld(), 0.1f, [](float alpha)
		{
		},
		[character]()
		{
			FVector parabolaLocation = character->GetMesh()->GetSocketLocation(TEXT("BowSocket"));

			AParabolaSpline* parabola = character->GetWorld()->SpawnActor<AParabolaSpline>(
			AParabolaSpline::StaticClass(),
			parabolaLocation,
			FRotator(0,0,0));

			parabola->InitializeParabola(0.05f, 5.f);

			parabola->AttachToActor(character, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
		
		});

		// 마우스 커서 교체

		if (auto* p = Cast<AMouseControlledPlayer>(character->GetWorld()->GetFirstPlayerController()->GetPawn()))
		{
			p->GetMouseManager()->SetMouseMode(EGameMouseState::Action);

			if (auto* cursor = Cast<UActionCursor>(p->GetMouseManager()->GetCursor()))
			{
				cursor->ShowActionDescription(action, 0);
			}
		}
	}

}

void URangedAttackAction::ExecuteAction(class AMoveCharacterBase* character, class UCharacterActionData* action)
{
	// 캐릭터 애니메이션 재생
	FString prepareID = action->ActionID + "_Execute";
	character->PlayAnimation(prepareID);

	AArrow* arrow = character->GetWorld()->SpawnActor<AArrow>(
	AArrow::StaticClass(),
	character->GetMesh()->GetSocketLocation(TEXT("BowSocket")),
	FRotator(0, 0, 0));
	
	FVector startPosition = character->GetMesh()->GetSocketLocation(TEXT("BowSocket"));
	AMoveCharacterBase* Target = action->Target.Num() > 0 ? action->Target[0] : nullptr;
	arrow->Initialize(action, character, Target);
	
	if (auto* playable = Cast<APlayableCharacterBase>(character))
	{
		playable->SetSplineCondition(true);
		TArray<AActor*> actors;
		character->GetAttachedActors(actors);
		FVector destination = FVector::ZeroVector;
		float height = 0.f;
	
		for (auto* actor : actors)
		{
			if (auto* cast = Cast<AParabolaSpline>(actor))
			{
				AParabolaSpline* spline = cast;
				spline->SetbIsExecute(true);
				destination = spline->GetDestination();
				height = spline->GetHeight();
			}
		}
		
		// 화살 이동
		UCustomTimer::SetTimer(character->GetWorld(), 0.4f, [destination, arrow, startPosition, height](float alpha)
		{
			if (arrow)
			{
				FVector dir = destination - arrow->GetActorLocation();
				FRotator rotation = dir.Rotation();
				arrow->SetActorRotation(rotation);
				arrow->SetActorLocation(UBGUtil::CalculateParabola(startPosition,destination, height, alpha), true);	
			}
		},
		[destination, arrow, startPosition, height]()
		{
			if (arrow)
			{
				arrow->SetActorLocation(UBGUtil::CalculateParabola(startPosition,destination, height, 1.0f), true);
			}
		});
	}
	Super::ExecuteAction(character, action);
}

void UMagicMissileAction::PrepareAction(class AMoveCharacterBase* character, class UCharacterActionData* action)
{
	Super::PrepareAction(character, action);

	// 캐릭터 애니메이션 재생
	FString prepareID = action->ActionID + "_Prepare";
	character->PlayAnimation(prepareID);

	if (auto* playable = Cast<APlayableCharacterBase>(character))
	{
		// 캐릭터 주변으로 범위 표시
		AAttackRange* decal = character->GetWorld()->SpawnActor<AAttackRange>(
			AAttackRange::StaticClass(),
			character->GetActorLocation(),
			FRotator(0, 0, 0));

		decal->SetDecalRange(action->MaxDistance);
		playable->SetSplineCondition(false);
	
		decal->AttachToActor(character, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
		
		// 마우스 커서 교체

		if (auto* p = Cast<AMouseControlledPlayer>(character->GetWorld()->GetFirstPlayerController()->GetPawn()))
		{
			p->GetMouseManager()->SetMouseMode(EGameMouseState::MultiTargetAction);
			p->GetPlayerUI()->ShowSpellInfo(action, playable);

			if (auto* cursor = Cast<UMultiTargetActionCursor>(p->GetMouseManager()->GetCursor()))
			{
				cursor->ShowActionDescription(action, 0);
			}
		}
	}
}

void UMagicMissileAction::ExecuteAction(class AMoveCharacterBase* character, class UCharacterActionData* action)
{
	if (auto* p = Cast<AMouseControlledPlayer>(character->GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		if (auto* cast = Cast<UMultiTargetActionCursor>(p->GetMouseManager()->GetCursor()))
		{
			cast->CurTargetNum = 0;
		}
		if (auto* playable = Cast<APlayableCharacterBase>(character))
		{
			playable->SetSplineCondition(true);
			p->GetPlayerUI()->SetSelectedCharacter(playable);
		}
	}
	
	Super::ExecuteAction(character, action);

	// 캐릭터 애니메이션 재생
	FString prepareID = action->ActionID + "_Execute";
	character->PlayAnimation(prepareID);

	int missilIndex = 0;

	// 미사일 발사
	for (auto* target : action->Target)
	{
		// 화살 이동
		UCustomTimer::SetTimer(character->GetWorld(), 0.5f * missilIndex, [](float alpha)
		{
		},
		[character, action, target]()
		{
			AMagicMissile* missile = character->GetWorld()->SpawnActor<AMagicMissile>(
			AMagicMissile::StaticClass(),
			character->GetMesh()->GetSocketLocation(TEXT("StaffSocket")),
			FRotator(0, 0, 0));
	
			missile->Initialize(action, character, target);
		});

		missilIndex++;
	}

	// 발사 뒤 action의 타겟 제거
	// maxCount 초기화
	action->Target.Empty();
	action->CurMaxTargetCount = action->MaxTargetCount;
	action->SkillCase = ESkillCase::SpellOne;
}
