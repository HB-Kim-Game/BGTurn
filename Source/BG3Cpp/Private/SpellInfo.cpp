// Fill out your copyright notice in the Description page of Project Settings.


#include "SpellInfo.h"

#include "ActionManager.h"
#include "CharacterActionData.h"
#include "BG3Enums.h"
#include "BG3GameMode.h"
#include "MoveCharacterBase.h"
#include "PlayableCharacterBase.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void USpellInfo::SetUpSpellInfo(class UCharacterActionData* action, class APlayableCharacterBase* character)
{
	if (nullptr == action)
	{
		CurTargetNum = 0;
		return;
	}
	
	ActionData = action;
	Character = character;
	if (!(nullptr != ActionData && ActionData == action))
	{
		DefaultSkillCase = action->SkillCase;
		CurTargetNum = 0;
	}
	
	IconImage->SetBrushFromTexture(action->Texture);

	if (DefaultSkillCase == ESkillCase::SpellTwo)
	{
		Spell1Button->SetIsEnabled(false);
	}
	else
	{
		Spell2Button->SetIsEnabled(true);
	}

	if (action->TargetCase == ETargetCase::Single)
	{
		TargetProgress->GetParent()->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		TargetProgress->GetParent()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ProjectileNum->SetText(FText::FromString(FString::FromInt(ActionData->CurMaxTargetCount)));
	}
}

void USpellInfo::ShowTargetProgress(int addNum)
{
	if (!ActionData) return;

	if (CurTargetNum + addNum > ActionData->MaxTargetCount) return;
	
	CurTargetNum += addNum;
	TargetMatDynamic->SetScalarParameterValue("Percent", static_cast<float>(CurTargetNum) / ActionData->CurMaxTargetCount);
}

void USpellInfo::NativeConstruct()
{
	Super::NativeConstruct();
	
	TargetMatDynamic = UMaterialInstanceDynamic::Create(TargetMaterial, this);
	TargetProgress->SetBrushFromMaterial(TargetMatDynamic);

	Spell1Button->OnClicked.AddDynamic(this, &USpellInfo::SpellOneClick);
	Spell2Button->OnClicked.AddDynamic(this, &USpellInfo::SpellTwoClick);
}

void USpellInfo::SpellOneClick()
{
	if (ActionData)
	{
		if (ActionData->SkillCase == ESkillCase::SpellOne)
		ActionData->CurMaxTargetCount = ActionData->MaxTargetCount;

		SetUpSpellInfo(ActionData, Character);
		ShowTargetProgress(0);
	}
}

void USpellInfo::SpellTwoClick()
{
	if (ActionData)
	{
		if (ActionData->SkillCase == ESkillCase::SpellTwo)
		{
			ActionData->CurMaxTargetCount = ActionData->MaxTargetCount;
		}
		else if (ActionData->SkillCase == ESkillCase::SpellOne)
		{
			ActionData->CurMaxTargetCount = ActionData->MaxTargetCount + ActionData->UpcastNum;
			ActionData->SkillCase = ESkillCase::SpellTwo;
		}

		SetUpSpellInfo(ActionData, Character);
		ShowTargetProgress(0);
	}
}

void USpellInfo::CheckButtonClick()
{
	if (auto* mode = Cast<ABG3GameMode>(GetWorld()->GetAuthGameMode()))
	{
		mode->ActionManager->ExecuteAction(ActionData, Cast<AMoveCharacterBase>(Character));
	}
}
