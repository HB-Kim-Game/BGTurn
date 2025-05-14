// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BG3Enums.h"
#include "ActionCountUI.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UActionCountUI : public UUserWidget
{
	GENERATED_BODY()

	public :
	
	UPROPERTY(meta = (BindWidget))
	class USizeBox* ActionBox;
	UPROPERTY(meta = (BindWidget))
	class USizeBox* BonusActionBox;
	
	UPROPERTY(meta = (BindWidget))
	class USizeBox* SpellLevel1Box;
	UPROPERTY(meta = (BindWidget))
	class USizeBox* SpellLevel2Box;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* ActionIconPanel;
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* BonusIconPanel;
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* Spell1IconPanel;
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* Spell2IconPanel;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UActionCountIcon> ActionCountIconClass;

	UPROPERTY(EditAnywhere)
	class UTexture2D* ActionIconImage;
	UPROPERTY(EditAnywhere)
	class UTexture2D* BonusIconImage;
	UPROPERTY(EditAnywhere)
	class UTexture2D* SpellIconImage;

	void ShowCharacterActionCount(class APlayableCharacterBase* character);
	void ShowSpellPanel(class APlayableCharacterBase* character);

	void CostAction(int currentCount);
	void CostBonus(int currentCount);
	void CostSpell1(int currentCount);
	void CostSpell2(int currentCount);
	
	void UseAction(int remainCount);
	void UseBonus(int remainCount);
	void UseSpell1(int remainCount);
	void UseSpell2(int remainCount);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY()
	TArray<class UHorizontalBox*> ActionIconParents;
	UPROPERTY()
	TArray<class UHorizontalBox*> BonusIconParents;
	UPROPERTY()
	TArray<class UHorizontalBox*> Spell1IconParents;
	UPROPERTY()
	TArray<class UHorizontalBox*> Spell2IconParents;

	UPROPERTY()
	TArray<class UActionCountIcon*> ActionIcons;
	UPROPERTY()
	TArray<class UActionCountIcon*> BonusIcons;
	UPROPERTY()
	TArray<class UActionCountIcon*> Spell1Icons;
	UPROPERTY()
	TArray<class UActionCountIcon*> Spell2Icons;

	void InitializeIcons();

	void AddIcon(EActionCase actionCase);
	void AddIcon(ESkillCase skillCase);

	void CreateIconWidget(EActionCase actionCase);
	void CreateIconWidget(ESkillCase skillCase);

	void ClearIcons();
};
