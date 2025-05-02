// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionBase.h"
#include "AttackRange.h"
#include "MoveCharacterBase.h"

void UActionBase::PrepareAction(AMoveCharacterBase* character, float distance)
{
}

void UActionBase::ExecuteAction(AMoveCharacterBase* character)
{
	
}

void UMeleeAction::PrepareAction(AMoveCharacterBase* character, float distance)
{
	Super::PrepareAction(character, distance);
	
	// 캐릭터 주변으로 범위 표시
	AAttackRange* decal = character->GetWorld()->SpawnActor<AAttackRange>(
		AAttackRange::StaticClass(),
		character->GetActorLocation(),
		FRotator(0, 0, 0));

	decal->SetDecalRange(distance);
	// 마우스 커서 교체
}

void UMeleeAction::ExecuteAction(AMoveCharacterBase* character)
{
	Super::ExecuteAction(character);

}

void USprintAction::PrepareAction(AMoveCharacterBase* character, float distance)
{
	Super::PrepareAction(character, distance);
}

void USprintAction::ExecuteAction(AMoveCharacterBase* character)
{
	Super::ExecuteAction(character);
	
}

void UFireBallAction::PrepareAction(AMoveCharacterBase* character, float distance)
{
	Super::PrepareAction(character, distance);
	// 캐릭터 주변으로 범위 표시
	AAttackRange* decal = character->GetWorld()->SpawnActor<AAttackRange>(
		AAttackRange::StaticClass(),
		character->GetActorLocation(),
		FRotator(0, 0, 0));

	decal->SetDecalRange(distance);
}

void UFireBallAction::ExecuteAction(AMoveCharacterBase* character)
{
	Super::ExecuteAction(character);
}
