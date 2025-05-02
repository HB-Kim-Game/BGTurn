// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionCountUI.h"

#include "ActionCountIcon.h"
#include "PlayableCharacterBase.h"
#include "Components/HorizontalBox.h"
#include "Components/SizeBox.h"
#include "Components/VerticalBox.h"

void UActionCountUI::ShowCharacterActionCount(class APlayableCharacterBase* character)
{
	if (ActionIcons.Num() < character->GetMaxTurnActionCount())
	{
		for (int i = 0; i < character->GetMaxTurnActionCount(); i++)
		{
			AddIcon(EActionCase::DefaultAction);
		}
	}
	else if (ActionIcons.Num() > character->GetMaxTurnActionCount())
	{
		for (int i = ActionIcons.Num() - 1; i > character->GetMaxTurnActionCount(); i--)
		{
			ActionIcons[i]->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (BonusIcons.Num() < character->GetMaxBonusActionCount())
	{
		for (int i = 0; i < character->GetMaxBonusActionCount(); i++)
		{
			AddIcon(EActionCase::BonusAction);
		}
	}
	else if (BonusIcons.Num() > character->GetMaxBonusActionCount())
	{
		for (int i = BonusIcons.Num() - 1; i > character->GetMaxBonusActionCount(); i--)
		{
			BonusIcons[i]->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	
	ShowSpellPanel(character->Status.CharClass == EGameCharacterClass::Wizard);

	for (auto* actionIcon : ActionIcons)
	{
		actionIcon->PlayNormalAnimation();
	}

	for (auto* bonusIcon : BonusIcons)
	{
		bonusIcon->PlayNormalAnimation();
	}
}

void UActionCountUI::ShowSpellPanel(bool condition)
{
	ActionBox->SetVisibility(ESlateVisibility::Hidden);
	BonusActionBox->SetVisibility(ESlateVisibility::Hidden);
	
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
	ActionBox->SetVisibility(ESlateVisibility::Visible);
	BonusActionBox->SetVisibility(ESlateVisibility::Visible);
}

void UActionCountUI::CostAction(int currentCount)
{
	ActionIcons[FMath::Clamp(currentCount-1, 0, ActionIcons.Num()-1)]->PlayCostAnimation();
}

void UActionCountUI::CostBonus(int currentCount)
{
	BonusIcons[FMath::Clamp(currentCount-1, 0, BonusIcons.Num()-1)]->PlayCostAnimation();
}

void UActionCountUI::UseAction(int remainCount)
{
	ActionIcons[FMath::Clamp(remainCount, 0, ActionIcons.Num())]->PlayUsedAnimation();
}

void UActionCountUI::UseBonus(int remainCount)
{
	BonusIcons[FMath::Clamp(remainCount, 0, BonusIcons.Num())]->PlayUsedAnimation();
}

void UActionCountUI::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeIcons();
}

void UActionCountUI::InitializeIcons()
{	TArray<UWidget*> actionIconParents = ActionIconPanel->GetAllChildren();
	TArray<UWidget*> bonusIconParents = BonusIconPanel->GetAllChildren();
	TArray<UWidget*> spell1IconParents = Spell1IconPanel->GetAllChildren();
	TArray<UWidget*> spell2IconParents = Spell2IconPanel->GetAllChildren();

	for (auto p1 : actionIconParents)
	{
		if (auto* cast = Cast<UHorizontalBox>(p1))
		{
			TArray<UWidget*> icons = cast->GetAllChildren();
			for (auto icon : icons)
			{
				if (auto* castIcon = Cast<UActionCountIcon>(icon))
				{
					ActionIcons.Add(castIcon);	
				}
			}
			ActionIconParents.Add(cast);
		}
	}

	for (auto p2 : bonusIconParents)
	{
		if (auto* cast = Cast<UHorizontalBox>(p2))
		{
			TArray<UWidget*> icons = cast->GetAllChildren();
			for (auto icon : icons)
			{
				if (auto* castIcon = Cast<UActionCountIcon>(icon))
				{
					BonusIcons.Add(castIcon);	
				}
			}
			BonusIconParents.Add(cast);
		}
	}

	for (auto p3 : spell1IconParents)
	{
		if (auto* cast = Cast<UHorizontalBox>(p3))
		{
			TArray<UWidget*> icons = cast->GetAllChildren();
			for (auto icon : icons)
			{
				if (auto* castIcon = Cast<UActionCountIcon>(icon))
				{
					Spell1Icons.Add(castIcon);	
				}
			}
			Spell1IconParents.Add(cast);
		}
	}

	for (auto p4 : spell2IconParents)
	{
		if (auto* cast = Cast<UHorizontalBox>(p4))
		{
			TArray<UWidget*> icons = cast->GetAllChildren();
			for (auto icon : icons)
			{
				if (auto* castIcon = Cast<UActionCountIcon>(icon))
				{
					Spell2Icons.Add(castIcon);	
				}
			}
			Spell2IconParents.Add(cast);
		}
	}
}

void UActionCountUI::AddIcon(EActionCase actionCase)
{
	CreateIconWidget(actionCase);
}

void UActionCountUI::CreateIconWidget(EActionCase actionCase)
{
	UActionCountIcon* item = Cast<UActionCountIcon>(CreateWidget(GetWorld(), ActionCountIconClass));

	if (actionCase == EActionCase::DefaultAction)
	{
		if (ActionIcons.Num() < 5)
		{
			for (auto* parent : ActionIconParents)
			{
				if (parent->GetChildrenCount() >= 2) continue;

				parent->AddChildToHorizontalBox(item);
				item->SetIconImage(ActionIconImage);
				item->PlayNormalAnimation();

				ActionIcons.Add(item);
			}
		}
	}
	else if (actionCase == EActionCase::BonusAction)
	{
		if (BonusIcons.Num() < 5)
		{
			for (auto* parent : BonusIconParents)
			{
				if (parent->GetChildrenCount() >= 2) continue;

				parent->AddChildToHorizontalBox(item);
				item->SetIconImage(BonusIconImage);
				item->PlayNormalAnimation();

				BonusIcons.Add(item);
			}
		}
	}
}

void UActionCountUI::ClearIcons()
{
	for (auto icon : ActionIcons) icon->RemoveFromParent();
	for (auto icon : BonusIcons) icon->RemoveFromParent();

	ActionIcons.Empty();
	BonusIcons.Empty();
}
