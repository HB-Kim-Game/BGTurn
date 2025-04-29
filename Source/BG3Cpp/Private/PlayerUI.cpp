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
}

void UPlayerUI::NativeConstruct()
{
	Super::NativeConstruct();

	HpMat = UMaterialInstanceDynamic::Create(HpMaterial, this);

	SelectedCharImage->SetBrushFromMaterial(HpMat);
}

void UPlayerUI::ShowSelectedCharHP(int32 curHp, int32 maxHp)
{
	HpMat->SetTextureParameterValue("CharacterImage", SelectedCharacter->Status.Portrait);
	HpMat->SetScalarParameterValue("Percentage", static_cast<float>(maxHp - curHp) / maxHp);
	
	FString str = FString::FromInt(curHp) + " / " + FString::FromInt(maxHp);
	
	SelectedCharHP->SetText(FText::FromString(str));
}
