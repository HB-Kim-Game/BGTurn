// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionBase.h"

#include "MoveCharacterBase.h"

void UActionBase::ExecuteAction(class AMoveCharacterBase* character)
{
	
}

void UMeleeAction::ExecuteAction(class AMoveCharacterBase* character)
{
	Super::ExecuteAction(character);

}

void USprintAction::ExecuteAction(class AMoveCharacterBase* character)
{
	Super::ExecuteAction(character);
	
}
