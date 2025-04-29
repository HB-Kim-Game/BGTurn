// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleTurnManager.h"

#include "MoveCharacterBase.h"
#include "TurnListViewer.h"
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
	
	// 임시 - 월드내의 MovableCharacterBase 타입의 객체들을 전부 가져와서 저장.
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMoveCharacterBase::StaticClass(), actors);
	for (AActor* actor : actors)
	{
		auto* cast = Cast<AMoveCharacterBase>(actor);
		if (cast)
		{
			Characters.Add(cast);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *cast->TableName.ToString());
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

