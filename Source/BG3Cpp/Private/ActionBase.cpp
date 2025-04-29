// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionBase.h"

#include "MoveCharacterBase.h"

void UActionBase::PrepareAction(class AMoveCharacterBase* character)
{
}

void UActionBase::ExecuteAction(class AMoveCharacterBase* character)
{
	
}

void UMeleeAction::PrepareAction(class AMoveCharacterBase* character)
{
	Super::PrepareAction(character);
}

void UMeleeAction::ExecuteAction(class AMoveCharacterBase* character)
{
	Super::ExecuteAction(character);

}

void USprintAction::PrepareAction(class AMoveCharacterBase* character)
{
	Super::PrepareAction(character);
}

void USprintAction::ExecuteAction(class AMoveCharacterBase* character)
{
	Super::ExecuteAction(character);
	
}

void UFireBallAction::PrepareAction(class AMoveCharacterBase* character)
{
	Super::PrepareAction(character);
}

void UFireBallAction::ExecuteAction(class AMoveCharacterBase* character)
{
	Super::ExecuteAction(character);
}
