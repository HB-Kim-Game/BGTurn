// Fill out your copyright notice in the Description page of Project Settings.


#include "BG3GameMode.h"

#include "ActionManager.h"
#include "BattleTurnManager.h"
#include "TurnListViewer.h"

void ABG3GameMode::SetAllCharacterOutline(bool condition)
{
	BattleManager->SetOutlineAllBattleCharacters(condition);
}

void ABG3GameMode::Initialize()
{
	ActionManager = NewObject<UActionManager>();
	ActionManager->InitializeAction();
	
	BattleManager = NewObject<UBattleTurnManager>(this, BattleMangerClass);
	BattleManager->RegisterComponent();

	BattleManager->StartBattle();
}

void ABG3GameMode::BeginPlay()
{
	Super::BeginPlay();
	
}
