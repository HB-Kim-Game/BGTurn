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
	
	UPROPERTY(meta = (BindWidget))
	class UImage* SelectedCharImage;
	
	UPROPERTY(meta = (BindWidget))
	class UActionListViewer* ActionListViewer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterialInterface* HpMaterial;

	UPROPERTY(editAnywhere, BlueprintReadWrite)
	TSubclassOf<class UActionButton> actionButtonClass;
	
	UPROPERTY()
	UMaterialInstanceDynamic* HpMat;

	void SetSelectedCharacter(class APlayableCharacterBase* character);
	
	virtual void NativeConstruct() override;

private:
	class APlayableCharacterBase* SelectedCharacter;
	
	void ShowSelectedCharHP(int32 curHp, int32 maxHp);
};
