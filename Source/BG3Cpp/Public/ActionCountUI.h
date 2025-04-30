// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
	class UVerticalBox* ActionIconPanel;
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* BonusIconPanel;
	
	UPROPERTY(meta = (BindWidget))
	class USizeBox* SpellLevel1Box;
	UPROPERTY(meta = (BindWidget))
	class USizeBox* SpellLevel2Box;
	
	TArray<class USizeBox*> SpellPanel;

	void ShowSpellPanel(bool condition);

protected:
	virtual void NativeConstruct() override;
};
