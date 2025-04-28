// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionButton.h"

#include "ActionHoverCursor.h"
#include "ActionManager.h"
#include "BG3GameMode.h"
#include "MouseControlledPlayer.h"
#include "MouseManager.h"
#include "MoveCharacterBase.h"
#include "PlayableCharacterBase.h"
#include "Components/Button.h"

void UActionButton::SetAction(FGameAction* action)
{
	Action = action;
}

void UActionButton::NativeConstruct()
{
	Super::NativeConstruct();

	Button->OnHovered.AddDynamic(this, &UActionButton::OnButtonHovered);
	Button->OnClicked.AddDynamic(this, &UActionButton::OnButtonClicked);
	Button->OnUnhovered.AddDynamic(this, &UActionButton::OnButtonUnhovered);

	Player = Cast<AMouseControlledPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

void UActionButton::OnButtonHovered()
{
	if (Action)
	{
		if (Player)
		{
			lastState = *(Player->GetMouseManager()->MouseStatePtr);
			
			Player->GetMouseManager()->SetMouseMode(EGameMouseState::ActionHover);

			auto* cursor = Cast<UActionHoverCursor>(Player->GetMouseManager()->GetCursor());
			if (cursor) cursor->ShowActionDetail(Action);
		}
	}
}

void UActionButton::OnButtonUnhovered()
{
	if (Player)
	{
		Player->GetMouseManager()->SetMouseMode(lastState);
	}
}

void UActionButton::OnButtonClicked()
{
	if (auto* gm = Cast<ABG3GameMode>(GetWorld()->GetAuthGameMode()))
	{
		if(gm->ActionManager)
		{
			if (auto* c = Player->GetPlayableCharacter())
			{
				bool condition = false;
				
				switch (Action->ActionCase)
				{
				case EActionCase::DefaultAction:
					condition = c->GetCurrentTurnActionCount() > 0;
					break;
				case EActionCase::BonusAction:
					condition = c->GetCurrentBonusActionCount() > 0;
					break;
				}

				if (condition)
				{
					auto* cast = Cast<AMoveCharacterBase>(c);
					gm->ActionManager->PrepareAction(*Action, cast);
				}
			}
		}
	};
}
