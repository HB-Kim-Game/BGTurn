// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionManager.h"
#include "ActionBase.h"
#include "CharacterActionData.h"
#include "MoveCharacterBase.h"

void UActionManager::InitializeAction()
{
	ActionMap.Add(TEXT("Melee"), NewObject<UMeleeAction>(this));
	ActionMap.Add(TEXT("Sprint"), NewObject<USprintAction>(this));
	ActionMap.Add(TEXT("Jump"), NewObject<UJumpAction>(this));
	ActionMap.Add(TEXT("FireBall"), NewObject<UFireBallAction>(this));
	ActionMap.Add(TEXT("RangedAttack"), NewObject<URangedAttackAction>(this));
	ActionMap.Add(TEXT("MagicMissile"), NewObject<UMagicMissileAction>(this));
}

void UActionManager::PrepareAction(UCharacterActionData* action, AMoveCharacterBase* character)
{
	if (nullptr == action) return;
	if (!IsValid(action)) return;
	if (auto* actionBase = ActionMap.Find(action->ActionID))
	{
		if (*actionBase)
		{
			(*actionBase)->PrepareAction(character, action);

			switch (action->ActionCase)
			{
			case EActionCase::DefaultAction:
				character->OnCharacterPrepareAction.Broadcast(action);
				break;
			case EActionCase::BonusAction:
				character->OnCharacterPrepareBonusAction.Broadcast(action);
				break;
			}

		}
	}
}

void UActionManager::ExecuteAction(UCharacterActionData* action, AMoveCharacterBase* character)
{
	if (auto* actionBase = ActionMap.Find(action->ActionID))
	{
		if (*actionBase)
		{
			(*actionBase)->ExecuteAction(character, action);

			switch (action->ActionCase)
			{
			case EActionCase::DefaultAction:
				character->OnCharacterAction.Broadcast(action);
				break;
			case EActionCase::BonusAction:
				character->OnCharacterBonusAction.Broadcast(action);
				break;
			}
		}
	}
}
