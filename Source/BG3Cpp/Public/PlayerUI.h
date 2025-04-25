// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerUI.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UPlayerUI : public UUserWidget
{
	GENERATED_BODY()

public :
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SelectedCharHP;

	void ShowSelectedCharHP(int32 curHp, int32 maxHp);
};
