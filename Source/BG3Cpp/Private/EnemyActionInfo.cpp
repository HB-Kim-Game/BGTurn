// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyActionInfo.h"

#include "CharacterActionData.h"
#include "CharacterStatus.h"
#include "MoveCharacterBase.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UEnemyActionInfo::ShowEnemyInfo(class AMoveCharacterBase* character, class UCharacterActionData* action)
{
	IconImage->SetBrushFromTexture(action->Texture);

	FString name = character->GetStatus()->GetName();
	FString actionName = action->DisplayName;
	
	CharacterName->SetText(FText::FromString(name));
	ActionName->SetText(FText::FromString(actionName));
}
