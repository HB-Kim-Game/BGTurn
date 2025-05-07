// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleTurnManager.h"

#include "AttackRange.h"
#include "BGUtil.h"
#include "DiceChecker.h"
#include "MouseControlledPlayer.h"
#include "MoveCharacterBase.h"
#include "NonPlayableCharacterBase.h"
#include "PlayableCharacterBase.h"
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

void UBattleTurnManager::Initialize()
{
	TurnList = Cast<UTurnListViewer>(CreateWidget(GetWorld(), TurnListClass));
	TurnList->AddToViewport(10);

	if (!Player)
	{
		Player = Cast<AMouseControlledPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
	}

	TurnList->SetVisibility(ESlateVisibility::Collapsed);
	
}

void UBattleTurnManager::StartBattle()
{
	Characters.Empty();

	TurnList->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	
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

			cast->OnTakeDefaultDamage.Add(FOnTakeDefaultDamage::FDelegate::CreateLambda([this]
			(float Damage, AMoveCharacterBase* damagedCharacter, AMoveCharacterBase* instigator)
			{
				this->TurnList->MoveCursor(0);
			}));
			
			if (auto* np = Cast<ANonPlayableCharacterBase>(cast))
			{
				np->OnCharacterTurnReceive.AddLambda([np, this]()
				{
					// 턴 받을 시, AI가 행동 시작하도록 구현해야함.
					Player->GetPlayerUI()->TurnEndButton->SetVisibility(ESlateVisibility::Hidden);
					Player->GetPlayerUI()->DefaultButton->SetVisibility(ESlateVisibility::Visible);
					np->TurnEnd();
				});

				FCharacterTurnData npData = FCharacterTurnData();
				npData.Character = np;
				
				Characters.Add(npData);
				continue;
			}
			if (auto* p = Cast<APlayableCharacterBase>(cast))
			{
				p->OnCharacterTurnEnd.AddLambda([p]()
				{
					TArray<AActor*> children;

					p->GetAttachedActors(children);

					for (auto* child : children)
					{
						if (auto* range = Cast<AAttackRange>(child))
						{
							range->Destroy();
						}
					}
				});
			}

			FCharacterTurnData data = FCharacterTurnData();
			data.Character = cast;
			
			Characters.Add(data);
		}
	}

	TurnList->FetchDatas(SortCharacters());
}

void UBattleTurnManager::SetOutlineAllBattleCharacters(bool condition)
{
	for (auto& character : Characters)
	{
		character.Character->SetOutline(condition);
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

TArray<AMoveCharacterBase*> UBattleTurnManager::SortCharacters()
{
	SortedCharacters.Empty();
	
	for (auto& data : Characters)
	{
		int32 initiative = FMath::Clamp(UDiceChecker::RollDice() + UBGUtil::CalculateBonus(data.Character->Status.Dex), 1, 20);
		data.Initiative = initiative;
		data.Character->ShowInitiative(data.Initiative);
	}

	Characters.Sort([](const FCharacterTurnData& A,const FCharacterTurnData& B)
	{
		return A.Initiative > B.Initiative;
	});

	for (auto& data : Characters)
	{
		SortedCharacters.Add(data.Character);
	}
	
	return SortedCharacters;
}

