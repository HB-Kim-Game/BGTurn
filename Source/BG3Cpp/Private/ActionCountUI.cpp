// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionCountUI.h"
#include "Components/SizeBox.h"
#include "Components/VerticalBox.h"

void UActionCountUI::ShowSpellPanel(bool condition)
{

	ActionIconPanel->GetParent()->GetParent()->SetVisibility(ESlateVisibility::Hidden);
	BonusIconPanel->GetParent()->GetParent()->SetVisibility(ESlateVisibility::Hidden);
	
	if (condition)
	{
		SpellLevel1Box->SetVisibility(ESlateVisibility::Visible);
		SpellLevel2Box->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SpellLevel1Box->SetVisibility(ESlateVisibility::Collapsed);
		SpellLevel2Box->SetVisibility(ESlateVisibility::Collapsed);
	}
	ActionIconPanel->GetParent()->GetParent()->SetVisibility(ESlateVisibility::Visible);
	BonusIconPanel->GetParent()->GetParent()->SetVisibility(ESlateVisibility::Visible);
}

void UActionCountUI::NativeConstruct()
{
	Super::NativeConstruct();
}
