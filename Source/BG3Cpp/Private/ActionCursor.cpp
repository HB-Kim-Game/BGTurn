// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionCursor.h"

#include "CharacterActionData.h"
#include "MouseControlledPlayer.h"
#include "MoveCharacterBase.h"
#include "PlayableCharacterBase.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"

UCharacterActionData* UActionCursor::GetAction() const
{
	return Action;
}

void UActionCursor::ShowActionDescription(UCharacterActionData* action, int percent,const FString& description)
{
	IconImage->SetBrushFromTexture(action->Texture);
	Action = action;
	
	if (percent > 90)
	{
		FString text = "<ActionPercentHigh>" + FString::FromInt(percent) + "%</>";
		Percent->SetText(FText::FromString(text));
	}
	else
	{
		if (percent <= 0)
		{
			Percent->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			Percent->SetVisibility(ESlateVisibility::Visible);
			FString text = "<ActionPercent>" + FString::FromInt(percent) + "%</>";
			Percent->SetText(FText::FromString(text));		
		}
	}

	if (description.IsEmpty())
	{
		Description->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		FString text = "<CursorDesc>" + description + "</CursorDesc>";
		Description->SetText(FText::FromString(text));
		Description->SetVisibility(ESlateVisibility::Visible);
	}
	
}

void UActionCursor::NativeConstruct()
{
	Super::NativeConstruct();

	player = Cast<AMouseControlledPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

void UActionCursor::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (nullptr == player && !player->IsValidLowLevel()) return;
	if (nullptr == Action && !Action->IsValidLowLevel()) return;

	auto* pc = GetWorld()->GetFirstPlayerController();
	if (pc)
	{
		FHitResult hit;
		if (pc->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_WorldDynamic), false, hit))
		{
			if (auto* character = Cast<AMoveCharacterBase>(hit.GetActor()))
			{
				float percent;
				switch (Action->SkillCase)
				{
					case ESkillCase::DefaultAttack:
						if (auto* playableCharacter = Cast<APlayableCharacterBase>(character))
						{
							if (player->GetPlayableCharacter() == playableCharacter) break;
						}
						percent = static_cast<float>(20 - character->Status.Defensive) / 20 * 100.f;
						ShowActionDescription(Action, static_cast<int>(percent));
						break;
					case ESkillCase::Buff:
						ShowActionDescription(Action, 100);
						break;
					case ESkillCase::SpellOne:
						if (auto* playableCharacter = Cast<APlayableCharacterBase>(character))
						{
							if (player->GetPlayableCharacter() == playableCharacter) break;
						}
						percent = static_cast<float>(20 - character->Status.Defensive) / 20 * 100.f;
						ShowActionDescription(Action, static_cast<int>(percent));
						break;
					case ESkillCase::SpellTwo:
						if (auto* playableCharacter = Cast<APlayableCharacterBase>(character))
						{
							if (player->GetPlayableCharacter() == playableCharacter) break;
						}
						percent = static_cast<float>(20 - character->Status.Defensive) / 20 * 100.f;
						ShowActionDescription(Action, static_cast<int>(percent));
						break;
				}
			}
			else
			{
				switch (Action->SkillCase)
				{
				case ESkillCase::DefaultAttack:
					ShowActionDescription(Action, 0);
					break;
				case ESkillCase::Buff:
					ShowActionDescription(Action, 100);
					break;
				case ESkillCase::SpellOne:
					ShowActionDescription(Action, 0);
					break;
				case ESkillCase::SpellTwo:
					ShowActionDescription(Action, 0);
					break;
				}	
			}
		}
	}
	
}
