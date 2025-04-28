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

	UE_LOG(LogTemp, Warning, TEXT("UMeleeAction::PrepareAction"));
}

void UMeleeAction::ExecuteAction(class AMoveCharacterBase* character)
{
	Super::ExecuteAction(character);

}

void USprintAction::ExecuteAction(class AMoveCharacterBase* character)
{
	Super::ExecuteAction(character);
	
}
