// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SelectObjectInfoUI.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API USelectObjectInfoUI : public UUserWidget
{
	GENERATED_BODY()

public:
	void ShowSelectObjectInfo(class AMoveCharacterBase* character);
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DisplayNameText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* LevelText;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPProgressBar;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HPText;
};
