// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BG3Enums.h"
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

	UPROPERTY(meta = (BindWidget))
	class USpellInfo* SpellInfo;

	UPROPERTY(meta = (BindWidget))
	class UImage* MoveProgress;

	UPROPERTY(meta = (BindWidget))
	class USelectObjectInfoUI* SelectObjectInfo;

	UPROPERTY(meta = (BindWidget))
	class UButton* TurnEndButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* DefaultButton;

	UPROPERTY(meta = (BindWidget))
	class UActionCountUI* ActionCountUI;

	UPROPERTY(meta = (BindWidget))
	class UEnemyActionInfo* EnemyActionInfo;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterialInterface* HpMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterialInterface* MoveMaterial;

	UPROPERTY(editAnywhere, BlueprintReadWrite)
	TSubclassOf<class UActionButton> actionButtonClass;
	
	UPROPERTY()
	UMaterialInstanceDynamic* HpMatDynamic;

	UPROPERTY()
	UMaterialInstanceDynamic* MoveMatDynamic;

	void SetSelectedCharacter(class APlayableCharacterBase* character);

	void ShowMoveProgress(float lastProgress, float resultProgress);

	void ShowSelectedObjectInfo(class AMoveCharacterBase* character);

	void ShowSpellInfo(class UCharacterActionData* action, APlayableCharacterBase* character);

	void ShowEnemyInfo(class AMoveCharacterBase* character, class UCharacterActionData* action);
	void CloseEnemyInfo();
	
	void ShowCost(class APlayableCharacterBase* character, EActionCase actionCase);
	void ShowUsed(class APlayableCharacterBase* character, EActionCase actionCase);

	void ShowCostSpell(class APlayableCharacterBase* character, ESkillCase skillCase);
	void ShowUsedSpell(class APlayableCharacterBase* character, ESkillCase skillCase);
	
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	class APlayableCharacterBase* SelectedCharacter;
	
	void ShowSelectedCharHP(int32 curHp, int32 maxHp);

	UFUNCTION()
	void ClickTurnEnd();
};
