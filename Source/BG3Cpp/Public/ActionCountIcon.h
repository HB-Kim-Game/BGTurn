// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ActionCountIcon.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UActionCountIcon : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetIconImage (class UTexture2D* texture);
	void PlayNormalAnimation();
	void PlayCostAnimation();
	void PlayUsedAnimation();
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* IconImage;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* NormalIcon;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* UsedIcon;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* CostIcon;
};
