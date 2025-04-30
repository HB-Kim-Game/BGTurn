// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleTurnManager.h"

#include "MouseControlledPlayer.h"
#include "MoveCharacterBase.h"
#include "NonPlayableCharacterBase.h"
#include "PlayerUI.h"
#include "TurnListViewer.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UBattleTurnManager::UBattleTurnManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UBattleTurnManager::StartBattle()
{
	Characters.Empty();

	TurnList = Cast<UTurnListViewer>(CreateWidget(GetWorld(), TurnListClass));
	TurnList->AddToViewport(10);

	if (!Player)
	{
		Player = Cast<AMouseControlledPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
	}
	
	// 임시 - 월드내의 MovableCharacterBase 타입의 객체들을 전부 가져와서 저장.
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMoveCharacterBase::StaticClass(), actors);
	for (AActor* actor : actors)
	{
		if (auto* cast = Cast<AMoveCharacterBase>(actor))
		{
			cast->OnCharacterTurnEnd.AddLambda([this]()
			{
				this->TurnList->MoveCursor(1);
			});

			cast->OnCharacterTurnReceive.AddLambda([this]()
			{
				auto* item = Cast<ISelectableObject>(this->TurnList->GetSelectedItem());
				Player->Select(item);
				auto* moveC = Cast<AMoveCharacterBase>(this->TurnList->GetSelectedItem());
				Player->Focus(FVector(moveC->GetActorLocation().X, moveC->GetActorLocation().Y, Player->GetActorLocation().Z));
			});
			
			if (auto* np = Cast<ANonPlayableCharacterBase>(cast))
			{
				np->OnCharacterTurnReceive.AddLambda([np, this]()
				{
					// 턴 받을 시, AI가 행동 시작하도록 구현해야함.
					Player->GetPlayerUI()->TurnEndButton->SetVisibility(ESlateVisibility::Hidden);
					Player->GetPlayerUI()->DefaultButton->SetVisibility(ESlateVisibility::Visible);
					np->TurnEnd();
				});

				Characters.Add(np);
				continue;
			};
			
			Characters.Add(cast);
		}
	}

	TurnList->FetchDatas(Characters);
}

void UBattleTurnManager::SetOutlineAllBattleCharacters(bool condition)
{
	for (auto* character : Characters)
	{
		character->SetOutline(condition);
	}
}


// Called when the game starts
void UBattleTurnManager::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UBattleTurnManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

