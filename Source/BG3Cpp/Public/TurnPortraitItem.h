// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ViewerItemBase.h"
#include "TurnPortraitItem.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UTurnPortraitItem : public UViewerItemBase
{
	GENERATED_BODY()

public :
	UPROPERTY(meta = (BindWidget))
	class UImage* Portrait;

	UPROPERTY(meta = (BindWidget))
	class UButton* SelectButton;

	UPROPERTY(meta = (BindWidget))
	class USizeBox* SizeBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterialInterface* HpMaterial;
	
	UPROPERTY()
	UMaterialInstanceDynamic* HpMatDynamic;
	
	class AMoveCharacterBase* GetFetchedCharacter() const;
	
	virtual void Selected() override;
	virtual void Deselected() override;
	virtual void FetchData(UObject* Data) override;

protected:
	UFUNCTION()
	virtual void OnButtonHovered();
	UFUNCTION()
	virtual void OnButtonUnhovered();
	UFUNCTION()
	virtual void OnButtonClicked();

	virtual void NativeConstruct() override;

	UPROPERTY()
	class AMouseControlledPlayer* Player;

	UPROPERTY()
	class AMoveCharacterBase* FetchedCharacter;

};
