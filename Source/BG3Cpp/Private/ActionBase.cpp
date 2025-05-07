// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionBase.h"

#include "ActionCursor.h"
#include "AttackRange.h"
#include "CharacterActionData.h"
#include "DiceChecker.h"
#include "BGUtil.h"
#include "BG3Enums.h"
#include "DamageUI.h"
#include "MouseControlledPlayer.h"
#include "MouseManager.h"
#include "MoveCharacterBase.h"
#include "PlayerUI.h"
#include "Kismet/GameplayStatics.h"

void UActionBase::PrepareAction(AMoveCharacterBase* character, UCharacterActionData* action)
{
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
	}
}

void UMeleeAction::PrepareAction(AMoveCharacterBase* character, UCharacterActionData* action)
{
	Super::PrepareAction(character, action);
	
	TArray<AActor*> actors;
	character->GetAttachedActors(actors);
	for (auto* actor : actors)
	{
		if (auto* cast = Cast<AAttackRange>(actor))
		{
			cast->Destroy();
		}
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
	
	if (nullptr == action->Target) return;
	if (!action->Target->IsValidLowLevel()) return;
	
	UE_LOG(LogTemp, Warning, TEXT("%s"), *action->Target->GetName());

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
		
		action->Target->GetDamageUI()->ShowDamage(action->Target->GetActorLocation(), damageResult);
		
		ExecuteActionHandle = action->Target->OnTakeDefaultDamage.Add(FOnTakeDefaultDamage::FDelegate::CreateLambda([action, this]
			(float Damage, AMoveCharacterBase* damagedCharacter, AMoveCharacterBase* instigator)
		{
			if (auto* p = Cast<AMouseControlledPlayer>(damagedCharacter->GetWorld()->GetFirstPlayerController()->GetPawn()))
			{
				p->GetPlayerUI()->ShowSelectedObjectInfo(damagedCharacter);
			}
			bool isSuccess = action->Target->OnTakeDefaultDamage.Remove(ExecuteActionHandle);
			UE_LOG(LogTemp, Warning, TEXT("%d"), isSuccess);
		}));
		
		UGameplayStatics::ApplyDamage(action->Target, damageResult, character->GetController(), character, UDamageType::StaticClass());	
		return;
	}

	// 무조건 무기에 숙련되어 있다는 기준, 숙련도 보너스 +2 를 함.
	// 공격 판정 성공
	if (diceResult + UBGUtil::CalculateBonus(statBonus) + 2 >= action->Target->Status.Defensive)
	{
		// 피해 굴림
		// 무기 기본 피해량(1d6을 기준으로 함.) + 능력치 수정값
		UE_LOG(LogTemp, Warning, TEXT("Success"));
		int8 damageResult = UDiceChecker::RollDice(6) + UBGUtil::CalculateBonus(statBonus);
		UE_LOG(LogTemp, Warning, TEXT("%d"), damageResult);

		action->Target->GetDamageUI()->ShowDamage(action->Target->GetActorLocation(), damageResult);

		ExecuteActionHandle = action->Target->OnTakeDefaultDamage.Add(FOnTakeDefaultDamage::FDelegate::CreateLambda([action, this]
			(float Damage, AMoveCharacterBase* damagedCharacter, AMoveCharacterBase* instigator)
		{
			if (auto* p = Cast<AMouseControlledPlayer>(damagedCharacter->GetWorld()->GetFirstPlayerController()->GetPawn()))
			{
				p->GetPlayerUI()->ShowSelectedObjectInfo(damagedCharacter);
			}
			bool isSuccess = action->Target->OnTakeDefaultDamage.Remove(ExecuteActionHandle);
			UE_LOG(LogTemp, Warning, TEXT("%d"), isSuccess);
		}));
		
		UGameplayStatics::ApplyDamage(action->Target, damageResult, character->GetController(), character, UDamageType::StaticClass());
		return;
	}

	FVector dir = character->GetActorLocation() - action->Target->GetActorLocation();
	dir.Z = 0;
	FRotator rotation = dir.Rotation();
	action->Target->SetActorRotation(rotation);
	action->Target->PlayAnimation(TEXT("Dodge"));
	action->Target->GetDamageUI()->ShowDamage(action->Target->GetActorLocation(), -1);
}

void USprintAction::PrepareAction(AMoveCharacterBase* character, UCharacterActionData* action)
{
	Super::PrepareAction(character, action);

	TArray<AActor*> actors;
	character->GetAttachedActors(actors);
	for (auto* actor : actors)
	{
		if (auto* cast = Cast<AAttackRange>(actor))
		{
			cast->Destroy();
		}
	}

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
	Super::PrepareAction(character, action);

	TArray<AActor*> actors;
	character->GetAttachedActors(actors);
	for (auto* actor : actors)
	{
		if (auto* cast = Cast<AAttackRange>(actor))
		{
			cast->Destroy();
		}
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

	// 마우스 커서 교체 (도약용)
	// 도약 지점 데칼 생성

	// 도약 거리 공식
	// 도약은 기본 4.5m + 힘 스탯 8에서 2 증가할때마다 1m 증가

	if (auto* p = Cast<AMouseControlledPlayer>(character->GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		
	}
}

void UJumpAction::ExecuteAction(AMoveCharacterBase* character, UCharacterActionData* action)
{
	Super::ExecuteAction(character, action);

	// 점프

	// 점프는 이동거리 3m를 소모함.
	character->AddMOV(-3.f, false);
	// 캐릭터 애니메이션 재생
	FString prepareID = action->ActionID + "_Execute";
	character->PlayAnimation(prepareID);
}

void UFireBallAction::PrepareAction(AMoveCharacterBase* character, UCharacterActionData* action)
{
	Super::PrepareAction(character, action);

	TArray<AActor*> actors;
	character->GetAttachedActors(actors);
	for (auto* actor : actors)
	{
		if (auto* cast = Cast<AAttackRange>(actor))
		{
			cast->Destroy();
		}
	}
	
	// 캐릭터 주변으로 범위 표시
	AAttackRange* decal = character->GetWorld()->SpawnActor<AAttackRange>(
		AAttackRange::StaticClass(),
		character->GetActorLocation(),
		FRotator(0, 0, 0));

	decal->SetDecalRange(action->MaxDistance);
	
	decal->AttachToActor(character, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
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

void UFireBallAction::ExecuteAction(AMoveCharacterBase* character, UCharacterActionData* action)
{
	Super::ExecuteAction(character, action);

	// 캐릭터 애니메이션 재생
	FString prepareID = action->ActionID + "_Execute";
	character->PlayAnimation(prepareID);
}
