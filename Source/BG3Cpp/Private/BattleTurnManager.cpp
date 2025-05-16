// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleTurnManager.h"

#include <list>

#include "AttackRange.h"
#include "BGUtil.h"
#include "CharacterStatus.h"
#include "CustomTimer.h"
#include "DiceChecker.h"
#include "MouseControlledPlayer.h"
#include "MoveCharacterBase.h"
#include "NonPlayableCharacterBase.h"
#include "ParabolaSpline.h"
#include "PlayableCharacterBase.h"
#include "PlayerUI.h"
#include "TurnCharacterList.h"
#include "TurnListViewer.h"
#include "TurnPortraitItem.h"
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
	TurnList->CharacterCount = actors.Num();
	for (AActor* actor : actors)
	{
		if (auto* cast = Cast<AMoveCharacterBase>(actor))
		{
			cast->OnCharacterTurnEnd.AddLambda([this, cast]()
			{
				if (this->TurnList->SelectedItems.Num() > 1)
				{
					for (auto* item : this->TurnList->SelectedItems)
					{
						if(item->GetFetchedCharacter() == cast) continue;
						if(item->GetFetchedCharacter()->GetIsTurn())
						{
							auto* selectableObject = Cast<ISelectableObject>(item->GetFetchedCharacter());
							Player->Select(selectableObject);
							auto* moveC = item->GetFetchedCharacter();
							Player->Focus(FVector(moveC->GetActorLocation().X, moveC->GetActorLocation().Y, Player->GetActorLocation().Z));
							return;
						}
					}	
				}
				this->TurnList->MoveCursor(1);
			});

			cast->OnCharacterTurnReceive.AddLambda([this]()
			{
				auto* item = Cast<ISelectableObject>(this->TurnList->SelectedItems[0]->GetFetchedCharacter());
				Player->Select(item);
				auto* moveC = this->TurnList->SelectedItems[0]->GetFetchedCharacter();
				Player->Focus(FVector(moveC->GetActorLocation().X, moveC->GetActorLocation().Y, Player->GetActorLocation().Z));	
			});

			cast->OnTakeDefaultDamage.Add(FOnTakeDefaultDamage::FDelegate::CreateLambda([this]
			(float Damage, AMoveCharacterBase* damagedCharacter, AMoveCharacterBase* instigator)
			{
				this->TurnList->MoveCursor(0, true);
			}));

			cast->OnDead.Add(FSimpleDelegate::CreateLambda([this, cast]()
			{
				auto* charList = TurnCharacterList.FindByPredicate([cast](const UTurnCharacterList* l)
				{
					return l->Characters.ContainsByPredicate([cast](const FCharacterTurnData& data)
					{
						return cast == data.Character;
					});
				});
				
				if (*charList)
				{
					int32 index = (*charList)->Characters.IndexOfByPredicate([cast](const FCharacterTurnData& data)
					{
						return data.Character == cast;
					});
					
					(*charList)->Characters.RemoveAt(index);
						
					if ((*charList)->Characters.Num() <= 0) TurnCharacterList.Remove(*charList);
				}

				int32 charindex = Characters.IndexOfByPredicate([cast](const FCharacterTurnData& data)
				{
					return data.Character == cast;
				});

				if (charindex != INDEX_NONE)
				{
					Characters.RemoveAt(charindex);
				}
				
				TurnList->FetchDatas(TurnCharacterList);
			}));
			
			if (auto* np = Cast<ANonPlayableCharacterBase>(cast))
			{
				np->OnCharacterTurnReceive.Add(FSimpleDelegate::CreateLambda([np, this]()
				{
					Player->GetPlayerUI()->TurnEndButton->SetVisibility(ESlateVisibility::Hidden);
					Player->GetPlayerUI()->DefaultButton->SetVisibility(ESlateVisibility::Visible);
					np->ThinkAction();
					Player->SetFocusEnemy(np);
				}));
				
				np->OnCharacterAction.Add(FOnCharacterAction::FDelegate::CreateLambda([this, np](UCharacterActionData* action)
				{
					Player->GetPlayerUI()->ShowEnemyInfo(np, action);
				}));

				np->OnCharacterBonusAction.Add(FOnCharacterBonusAction::FDelegate::CreateLambda([this, np](UCharacterActionData* action)
				{
					Player->GetPlayerUI()->ShowEnemyInfo(np, action);
				}));

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
						if (auto* parabola = Cast<AParabolaSpline>(child))
						{
							TArray<AActor*> parabolaChildren;
							parabola->GetAttachedActors(parabolaChildren);
							for (auto* parabolaChild : parabolaChildren)
							{
								parabolaChild->Destroy();
							}
							parabola->Destroy();
						}
					}
				});

				p->OnCharacterTurnReceive.AddLambda([this]()
				{
					Player->GetPlayerUI()->CloseEnemyInfo();
					Player->SetFocusEnemy(nullptr);
				});
			}

			FCharacterTurnData data = FCharacterTurnData();
			data.Character = cast;
			
			Characters.Add(data);
		}
	}

	TurnList->FetchDatas(SortCharacters());
	TurnList->MoveCursor(0);
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

TArray<UTurnCharacterList*> UBattleTurnManager::SortCharacters()
{
	TurnCharacterList.Empty();
	
	for (auto& data : Characters)
	{
		int32 initiative = FMath::Max(UDiceChecker::RollDice(4) + UBGUtil::CalculateBonus(data.Character->Status.Dex), 1);
		data.Initiative = initiative;
		UE_LOG(LogTemp, Warning, TEXT("%s"), *data.Character->GetStatus()->GetName());
		UE_LOG(LogTemp, Warning, TEXT("Initiative : %d"), initiative);
		if (TurnCharacterList.Num() <= 0)
		{
			UTurnCharacterList* list = NewObject<UTurnCharacterList>(this);
			list->Characters.Add(data);
			TurnCharacterList.Add(list);
			UE_LOG(LogTemp, Warning, TEXT("New"));
		}
		else
		{
			if (auto* charList = TurnCharacterList.FindByPredicate([initiative, data](const UTurnCharacterList* list)
			{
				return list->Characters[0].Initiative == initiative && list->Characters[0].Character->GetClass() == data.Character->GetClass();
			}))
			{
				(*charList)->Characters.Add(data);
				UE_LOG(LogTemp, Warning, TEXT("Add"));
			}
			else
			{
				UTurnCharacterList* list = NewObject<UTurnCharacterList>(this);
				list->Characters.Add(data);
				TurnCharacterList.Add(list);
				UE_LOG(LogTemp, Warning, TEXT("New"));
			}
		}
		data.Character->ShowInitiative(data.Initiative);
	}

	TurnCharacterList.Sort([](const UTurnCharacterList& A, const UTurnCharacterList& B)
	{
		return A.Characters[0].Initiative > B.Characters[0].Initiative;
	});

	UE_LOG(LogTemp, Warning, TEXT("TurnCharacterListCount : %d"), TurnCharacterList.Num());
	
	return TurnCharacterList;
}

