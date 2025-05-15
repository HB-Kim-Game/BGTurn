// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyActionInfo.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UEnemyActionInfo : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UImage* IconImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CharacterName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ActionName;

	void ShowEnemyInfo(class AMoveCharacterBase* character, class UCharacterActionData* action);
};
