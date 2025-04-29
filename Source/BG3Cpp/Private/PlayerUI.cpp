// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerUI.h"

#include "CharacterStatus.h"
#include "PlayableCharacterBase.h"
#include "ActionListViewer.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UPlayerUI::SetSelectedCharacter(APlayableCharacterBase* character)
{
	SelectedCharacter = character;

	ActionListViewer->FetchDatas(character->GetActions());
	
	ShowSelectedCharHP(*SelectedCharacter->CurHPPtr, SelectedCharacter->GetStatus()->GetHp());
	float progress = SelectedCharacter->GetCurrentMOV() / SelectedCharacter->Status.MOV;
	ShowMoveProgress(progress, progress);
}

void UPlayerUI::ShowMoveProgress(float lastProgress, float resultProgress)
{
	MoveMatDynamic->SetScalarParameterValue("LastPercent", lastProgress);
	MoveMatDynamic->SetScalarParameterValue("Percent", resultProgress);
}

void UPlayerUI::NativeConstruct()
{
	Super::NativeConstruct();

	HpMatDynamic = UMaterialInstanceDynamic::Create(HpMaterial, this);

	SelectedCharImage->SetBrushFromMaterial(HpMatDynamic);

	MoveMatDynamic = UMaterialInstanceDynamic::Create(MoveMaterial, this);

	MoveProgress->SetBrushFromMaterial(MoveMatDynamic);
}

void UPlayerUI::ShowSelectedCharHP(int32 curHp, int32 maxHp)
{
	HpMatDynamic->SetTextureParameterValue("CharacterImage", SelectedCharacter->Status.Portrait);
	HpMatDynamic->SetScalarParameterValue("Percentage", static_cast<float>(maxHp - curHp) / maxHp);
	
	FString str = FString::FromInt(curHp) + " / " + FString::FromInt(maxHp);
	
	SelectedCharHP->SetText(FText::FromString(str));
}
