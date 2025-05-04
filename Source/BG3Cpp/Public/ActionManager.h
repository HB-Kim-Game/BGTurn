// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionManager.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UActionManager : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TMap<FString, class UActionBase*> ActionMap;
	
	void InitializeAction();

	void PrepareAction(class UCharacterActionData* action, class AMoveCharacterBase* character);
	void ExecuteAction(class UCharacterActionData* action, class AMoveCharacterBase* character);
	
};
