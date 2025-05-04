// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InitiativeUI.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UInitiativeUI : public UUserWidget
{
	GENERATED_BODY()

public:
	void ShowInitiative(FVector location, int32 initiativeNum);

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* InitiativeNum;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* Appear;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* Disappear;

	virtual void NativeConstruct() override;

	FTimerHandle DisappearHandle;
	FTimerHandle PositionHandle;

	FWidgetAnimationDynamicEvent DisappearEvent;

	UFUNCTION()
	void DisappearEnd();
};
