// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionBase.h"

#include "ActionCursor.h"
#include "AttackRange.h"
#include "CharacterActionData.h"
#include "MouseControlledPlayer.h"
#include "MouseManager.h"
#include "MoveCharacterBase.h"

void UActionBase::PrepareAction(AMoveCharacterBase* character, UCharacterActionData* action)
{
}

void UActionBase::ExecuteAction(AMoveCharacterBase* character)
{
	
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

void UMeleeAction::ExecuteAction(AMoveCharacterBase* character)
{
	Super::ExecuteAction(character);

	UE_LOG(LogTemp, Warning, TEXT("Melee"));

}

void USprintAction::PrepareAction(AMoveCharacterBase* character, UCharacterActionData* action)
{
	Super::PrepareAction(character, action);
}

void USprintAction::ExecuteAction(AMoveCharacterBase* character)
{
	Super::ExecuteAction(character);
	
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

void UFireBallAction::ExecuteAction(AMoveCharacterBase* character)
{
	Super::ExecuteAction(character);
}
