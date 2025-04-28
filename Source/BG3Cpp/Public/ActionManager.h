// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BG3Struct.h"
#include "ActionManager.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UActionManager : public UObject
{
	GENERATED_BODY()

public:
	
	TMap<FString, class UActionBase*> ActionMap;
	
	void InitializeAction();

	void PrepareAction(FGameAction action, class AMoveCharacterBase* character);
	void ExecuteAction(FGameAction action, class AMoveCharacterBase* character);
	
};
