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
	
	ShowSpellPanel(character);

	for (auto* actionIcon : ActionIcons)
	{
		actionIcon->PlayNormalAnimation();
	}

	for (auto* bonusIcon : BonusIcons)
	{
		bonusIcon->PlayNormalAnimation();
	}
}

void UActionCountUI::ShowSpellPanel(class APlayableCharacterBase* character)
{
	ActionBox->SetVisibility(ESlateVisibility::Hidden);
	BonusActionBox->SetVisibility(ESlateVisibility::Hidden);
	
	if (character->Status.DefaultSpellOneCount > 0)
	{
		SpellLevel1Box->SetVisibility(ESlateVisibility::Visible);
		SpellLevel2Box->SetVisibility(ESlateVisibility::Visible);
		
		if (Spell1Icons.Num() < character->Status.DefaultSpellOneCount)
		{
			for (int i = 0; i < character->Status.DefaultSpellOneCount; i++)
			{
				AddIcon(ESkillCase::SpellOne);
			}
		}
		else if (Spell1Icons.Num() > character->Status.DefaultSpellOneCount)
		{
			for (int i = Spell1Icons.Num() - 1; i > character->Status.DefaultSpellOneCount; i--)
			{
				Spell1Icons[i]->SetVisibility(ESlateVisibility::Collapsed);
			}
		}

		if (Spell2Icons.Num() < character->Status.DefaultSpellTwoCount)
		{
			for (int i = 0; i < character->Status.DefaultSpellTwoCount; i++)
			{
				AddIcon(ESkillCase::SpellTwo);
			}
		}
		else if (Spell2Icons.Num() > character->Status.DefaultSpellTwoCount)
		{
			for (int i = Spell2Icons.Num() - 1; i > character->Status.DefaultSpellTwoCount; i--)
			{
				Spell2Icons[i]->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
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

void UActionCountUI::CostSpell1(int currentCount)
{
	Spell1Icons[FMath::Clamp(currentCount-1, 0, Spell1Icons.Num()-1)]->PlayCostAnimation();
}

void UActionCountUI::CostSpell2(int currentCount)
{
	Spell2Icons[FMath::Clamp(currentCount-1, 0, Spell2Icons.Num()-1)]->PlayCostAnimation();
}

void UActionCountUI::UseAction(int remainCount)
{
	ActionIcons[FMath::Clamp(remainCount, 0, ActionIcons.Num()-1)]->PlayUsedAnimation();
}

void UActionCountUI::UseBonus(int remainCount)
{
	BonusIcons[FMath::Clamp(remainCount, 0, BonusIcons.Num()-1)]->PlayUsedAnimation();
}

void UActionCountUI::UseSpell1(int remainCount)
{
	Spell1Icons[FMath::Clamp(remainCount, 0, Spell1Icons.Num()-1)]->PlayUsedAnimation();
}

void UActionCountUI::UseSpell2(int remainCount)
{
	Spell2Icons[FMath::Clamp(remainCount, 0, Spell2Icons.Num()-1)]->PlayUsedAnimation();
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
			ActionIconParents.Add(cast);
		}
	}

	for (auto p2 : bonusIconParents)
	{
		if (auto* cast = Cast<UHorizontalBox>(p2))
		{
			BonusIconParents.Add(cast);
		}
	}

	for (auto p3 : spell1IconParents)
	{
		if (auto* cast = Cast<UHorizontalBox>(p3))
		{
			Spell1IconParents.Add(cast);
		}
	}

	for (auto p4 : spell2IconParents)
	{
		if (auto* cast = Cast<UHorizontalBox>(p4))
		{
			Spell2IconParents.Add(cast);
		}
	}

	ActionIcons.Empty();
	BonusIcons.Empty();
	Spell1Icons.Empty();
	Spell2Icons.Empty();
}

void UActionCountUI::AddIcon(EActionCase actionCase)
{
	CreateIconWidget(actionCase);
}

void UActionCountUI::AddIcon(ESkillCase skillCase)
{
	CreateIconWidget(skillCase);
}

void UActionCountUI::CreateIconWidget(EActionCase actionCase)
{
	UActionCountIcon* item = Cast<UActionCountIcon>(CreateWidget(GetWorld(), ActionCountIconClass));

	if (actionCase == EActionCase::DefaultAction)
	{
		if (ActionIcons.Num() < 5)
		{
			int parentNum = ActionIconParents.Num() / 2;
			ActionIconParents[parentNum]->AddChildToHorizontalBox(item);
			item->SetIconImage(ActionIconImage);
			item->PlayNormalAnimation();

			ActionIcons.Add(item);
		}
	}
	else if (actionCase == EActionCase::BonusAction)
	{
		if (BonusIcons.Num() < 5)
		{
			int parentNum = BonusIconParents.Num() / 2;
			BonusIconParents[parentNum]->AddChildToHorizontalBox(item);
			item->SetIconImage(BonusIconImage);
			item->PlayNormalAnimation();
			  	
			BonusIcons.Add(item);
		}
	}
}

void UActionCountUI::CreateIconWidget(ESkillCase skillCase)
{
	UActionCountIcon* item = Cast<UActionCountIcon>(CreateWidget(GetWorld(), ActionCountIconClass));

	if (skillCase == ESkillCase::SpellOne)
	{
		if (Spell1Icons.Num() < 5)
		{
			int parentNum = Spell1Icons.Num() / 2;

			Spell1IconParents[parentNum]->AddChildToHorizontalBox(item);
			item->SetIconImage(SpellIconImage);
			item->PlayNormalAnimation();
			
			Spell1Icons.Add(item);
		}
	}
	else if (skillCase == ESkillCase::SpellTwo)
	{
		if (Spell2Icons.Num() < 5)
		{
			int parentNum = Spell2Icons.Num() / 2;

			Spell2IconParents[parentNum]->AddChildToHorizontalBox(item);
			item->SetIconImage(SpellIconImage);
			item->PlayNormalAnimation();

			Spell2Icons.Add(item);
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
