// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageUI.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UDamageUI : public UUserWidget
{
	GENERATED_BODY()

public:
	void ShowDamage(FVector location, int32 damage);

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DamageText;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* Appear;

	FTimerHandle PositionHandle;
	
	FWidgetAnimationDynamicEvent FOnFinished;

	UFUNCTION()
	void OnFinished();

	virtual void NativeConstruct() override;
};
