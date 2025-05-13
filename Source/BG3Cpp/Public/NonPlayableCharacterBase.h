// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MoveCharacterBase.h"
#include "NonPlayableCharacterBase.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API ANonPlayableCharacterBase : public AMoveCharacterBase
{
	GENERATED_BODY()

public:
	virtual void Selected() override;
	virtual void Deselected() override;

	void ThinkAction();
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	class ABG3GameMode* Mode;

	UPROPERTY()
	TArray<class AMoveCharacterBase*> EnemyCharacters;

	FDelegateHandle MoveDelegateHandle;
};
