// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BG3Enums.h"
#include "ViewerItemBase.h"
#include "ActionButton.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UActionButton : public UViewerItemBase
{
	GENERATED_BODY()

	public :
	UPROPERTY(meta = (BindWidget))
	class UImage* IconImage;

	UPROPERTY(meta = (BindWidget))
	class UButton* Button;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	virtual void OnButtonHovered();
	UFUNCTION()
	virtual void OnButtonUnhovered();
	UFUNCTION()
	virtual void OnButtonClicked();

	virtual void Selected() override;
	virtual void Deselected() override;
	virtual void FetchData(UObject* Data) override;

private:
	EGameMouseState lastState;
	UPROPERTY()
	class UCharacterActionData* Action;

	UPROPERTY()
	class AMouseControlledPlayer* Player;

	bool bIsSelected = false;
};
