// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerUI.h"

#include "ActionButton.h"
#include "CharacterStatus.h"
#include "PlayableCharacterBase.h"
#include "Components/GridPanel.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UPlayerUI::SetSelectedCharacter(APlayableCharacterBase* character)
{
	SelectedCharacter = character;

	ShowSelectedCharHP(*SelectedCharacter->CurHPPtr, SelectedCharacter->GetStatus()->GetHp());
}

void UPlayerUI::NativeConstruct()
{
	Super::NativeConstruct();

	HpMat = UMaterialInstanceDynamic::Create(HpMaterial, this);
	SelectedCharImage->SetBrushFromMaterial(HpMat);
	
	auto* testButton = Cast<UActionButton>(CreateWidget(GetWorld(), actionButtonClass));
	ActionButtonPanel->AddChild(testButton);
	
	FGameAction* testAction = new FGameAction();
	testAction->DisplayName = TEXT("근거리공격");
	testAction->ActionCase = EActionCase::DefaultAction;
	testAction->Description = TEXT("근거리공격 근거리공격 근거리공격 근거리공격 근거리공격 근거리공격");
	testAction->ActionID = "Melee";
	
	testButton->SetAction(testAction);
}

void UPlayerUI::ShowSelectedCharHP(int32 curHp, int32 maxHp)
{
	HpMat->SetScalarParameterValue("Percentage", static_cast<float>(maxHp - curHp) / maxHp);
	
	FString str = FString::FromInt(curHp) + " / " + FString::FromInt(maxHp);
	
	SelectedCharHP->SetText(FText::FromString(str));
}
