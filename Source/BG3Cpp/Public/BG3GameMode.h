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

public:
	UPROPERTY()
	class UActionManager* ActionManager;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UBattleTurnManager> BattleMangerClass;

	UPROPERTY()
	class UBattleTurnManager* BattleManager;

	void SetAllCharacterOutline(bool condition);
	
	void Initialize();
	
	protected:
	
	virtual void BeginPlay() override;
};
