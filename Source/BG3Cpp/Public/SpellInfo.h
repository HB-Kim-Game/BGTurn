// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BG3Enums.h"
#include "Blueprint/UserWidget.h"
#include "SpellInfo.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API USpellInfo : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetUpSpellInfo(class UCharacterActionData* action, class APlayableCharacterBase* character);

	void ShowTargetProgress(int addNum);

	int CurTargetNum = 0;
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* IconImage;
	UPROPERTY(meta = (BindWidget))
	class UButton* Spell1Button;
	UPROPERTY(meta = (BindWidget))
	class UButton* Spell2Button;

	UPROPERTY(meta = (BindWidget))
	class UImage* TargetProgress;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ProjectileNum;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ProjectileName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterialInterface* TargetMaterial;
	
	UPROPERTY()
	UMaterialInstanceDynamic* TargetMatDynamic;
	
	UPROPERTY()
	class UCharacterActionData* ActionData;

	UPROPERTY()
	class APlayableCharacterBase* Character;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void SpellOneClick();

	UFUNCTION()
	void SpellTwoClick();
	
	UFUNCTION()
	void CheckButtonClick();

	ESkillCase DefaultSkillCase = ESkillCase::SpellOne;
};
