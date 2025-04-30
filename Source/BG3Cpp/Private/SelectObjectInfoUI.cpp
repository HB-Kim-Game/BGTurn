// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveCharacterBase.h"
#include "SelectObjectInfoUI.h"

#include "CharacterStatus.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USelectObjectInfoUI::ShowSelectObjectInfo(AMoveCharacterBase* character)
{
	DisplayNameText->SetText(FText::FromString(character->GetStatus()->GetName()));

	FString level = "Lv. " + FString::FromInt(character->Status.Level);
	
	LevelText->SetText(FText::FromString(level));
	
	float progress = static_cast<float>(*character->CurHPPtr) / character->GetStatus()->GetHp();
	HPProgressBar->SetPercent(progress);

	FString hp = FString::FromInt(*character->CurHPPtr) + "/" +  FString::FromInt(character->GetStatus()->GetHp());

	HPText->SetText(FText::FromString(hp));
}
