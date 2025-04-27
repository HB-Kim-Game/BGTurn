// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BG3GameMode.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API ABG3GameMode : public AGameModeBase
{
	GENERATED_BODY()

	protected:
	
	class UActionManager* ActionManager;
	
	virtual void BeginPlay() override;
};
