// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerUI.h"

#include "ActionCountUI.h"
#include "CharacterStatus.h"
#include "PlayableCharacterBase.h"
#include "ActionListViewer.h"
#include "SelectObjectInfoUI.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UPlayerUI::SetSelectedCharacter(APlayableCharacterBase* character)
{
	SelectedCharacter = character;
	ActionListViewer->FetchDatas(SelectedCharacter->GetActions());
	
	ShowSelectedCharHP(*SelectedCharacter->CurHPPtr, SelectedCharacter->GetStatus()->GetHp());
	float progress = SelectedCharacter->GetCurrentMOV() / SelectedCharacter->Status.MOV;
	ShowMoveProgress(progress, progress);

	ShowSelectedObjectInfo(SelectedCharacter);

	if (character->GetIsTurn())
	{
		TurnEndButton->SetVisibility(ESlateVisibility::Visible);
		DefaultButton->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		TurnEndButton->SetVisibility(ESlateVisibility::Hidden);
		DefaultButton->SetVisibility(ESlateVisibility::Visible);
	}

	ActionCountUI->ShowCharacterActionCount(SelectedCharacter);

	if (SelectedCharacter->GetCurrentTurnActionCount() != SelectedCharacter->GetMaxTurnActionCount())
	{
		ShowUsed(SelectedCharacter, EActionCase::DefaultAction);
	}
	if (SelectedCharacter->GetCurrentBonusActionCount() != SelectedCharacter->GetMaxBonusActionCount())
	{
		ShowUsed(SelectedCharacter, EActionCase::BonusAction);
	}
}

void UPlayerUI::ShowMoveProgress(float lastProgress, float resultProgress)
{
	MoveMatDynamic->SetScalarParameterValue("LastPercent", lastProgress);
	MoveMatDynamic->SetScalarParameterValue("Percent", resultProgress);
}

void UPlayerUI::ShowSelectedObjectInfo(AMoveCharacterBase* character)
{
	SelectObjectInfo->SetVisibility(ESlateVisibility::Visible);
	SelectObjectInfo->ShowSelectObjectInfo(character);
}

void UPlayerUI::ShowCost(APlayableCharacterBase* character, EActionCase actionCase)
{
	switch (actionCase)
	{
	case EActionCase::DefaultAction:
		ActionCountUI->CostAction(character->GetCurrentTurnActionCount());	
		break;
		case EActionCase::BonusAction:
		ActionCountUI->CostBonus(character->GetCurrentBonusActionCount());	
		break;
	}
}

void UPlayerUI::ShowUsed(APlayableCharacterBase* character, EActionCase actionCase)
{
	switch (actionCase)
	{
	case EActionCase::DefaultAction:
		ActionCountUI->UseAction(character->GetCurrentTurnActionCount());	
		break;
	case EActionCase::BonusAction:
		ActionCountUI->UseBonus(character->GetCurrentBonusActionCount());	
		break;
	}

	ActionListViewer->FetchDatas(character->GetActions());
}

void UPlayerUI::NativeConstruct()
{
	Super::NativeConstruct();

	HpMatDynamic = UMaterialInstanceDynamic::Create(HpMaterial, this);

	SelectedCharImage->SetBrushFromMaterial(HpMatDynamic);

	MoveMatDynamic = UMaterialInstanceDynamic::Create(MoveMaterial, this);

	MoveProgress->SetBrushFromMaterial(MoveMatDynamic);

	TurnEndButton->OnClicked.AddDynamic(this, &UPlayerUI::ClickTurnEnd);
	
	SelectObjectInfo->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerUI::ShowSelectedCharHP(int32 curHp, int32 maxHp)
{
	HpMatDynamic->SetTextureParameterValue("CharacterImage", SelectedCharacter->Status.Portrait);
	HpMatDynamic->SetScalarParameterValue("Percentage", static_cast<float>(maxHp - curHp) / maxHp);
	UE_LOG(LogTemp, Warning, TEXT("%f"), static_cast<float>(maxHp - curHp) / maxHp);
	
	FString str = FString::FromInt(curHp) + "/" + FString::FromInt(maxHp);
	
	SelectedCharHP->SetText(FText::FromString(str));
}

void UPlayerUI::ClickTurnEnd()
{
	if (SelectedCharacter)
	{
		SelectedCharacter->TurnEnd();
	}
}
