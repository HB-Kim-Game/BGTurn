// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BG3Struct.h"
#include "BG3Enums.h"
#include "Blueprint/UserWidget.h"
#include "ActionButton.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UActionButton : public UUserWidget
{
	GENERATED_BODY()

	public :
	UPROPERTY(meta = (BindWidget))
	class UImage* IconImage;

	UPROPERTY(meta = (BindWidget))
	class UButton* Button;
	
	void SetAction(FGameAction* action);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	virtual void OnButtonHovered();
	UFUNCTION()
	virtual void OnButtonUnhovered();
	UFUNCTION()
	virtual void OnButtonClicked();

private:
	EGameMouseState lastState;
	FGameAction* Action;

	class AMouseControlledPlayer* Player;
};
