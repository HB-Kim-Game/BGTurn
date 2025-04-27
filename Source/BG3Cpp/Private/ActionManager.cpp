// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionManager.h"
#include "ActionBase.h"
#include "BG3Struct.h"
#include "MoveCharacterBase.h"

void UActionManager::InitializeAction()
{
	ActionMap.Add("Melee", NewObject<UMeleeAction>(this));
	ActionMap.Add("Sprint", NewObject<USprintAction>(this));
}

void UActionManager::ExecuteAction(FGameAction action, AMoveCharacterBase* character)
{
	if (auto* actionBase = ActionMap.Find(action.ActionID))
	{
		if (*actionBase)
		{
			(*actionBase)->ExecuteAction(character);

			switch (action.ActionCase)
			{
			case EActionCase::DefaultAction:
				character->OnCharacterAction.Broadcast();
				break;
			case EActionCase::BonusAction:
				character->OnCharacterBonusAction.Broadcast();
				break;
			}
		}
	}
}
