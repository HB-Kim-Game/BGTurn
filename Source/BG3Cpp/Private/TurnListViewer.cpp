
#include "TurnListViewer.h"

#include "BGUtil.h"
#include "CharacterStatus.h"
#include "MoveCharacterBase.h"
#include "NonPlayableCharacterBase.h"
#include "TurnCharacterList.h"
#include "TurnPortraitItem.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/HorizontalBox.h"

void UTurnListViewer::NativeConstruct()
{
	Super::NativeConstruct();

	OnRefreshList.AddLambda([this]()
	{
		CachedList.Empty();
	
		for (auto* data : FetchedDatas)
		{
			if (auto* cast = Cast<UTurnCharacterList>(data))
			{
				CachedList.Add(cast);
			}
		}	
	});
}

void UTurnListViewer::OnDataFetched()
{
	Super::OnDataFetched();

	MoveCursor(0, true);
}

void UTurnListViewer::InitializeItem()
{
	int32 itemCount = ItemPoolCount >= CharacterCount ? CharacterCount : ItemPoolCount;
	
	for (int32 i = 0; i < itemCount; i++)
	{
		UViewerItemBase* item = Cast<UViewerItemBase>(CreateWidget(GetWorld(), ItemClass));
		if (item)
		{
			SpawnItems.Add(item);
			if (auto* cast = Cast<UTurnPortraitItem>(item))
			{
				CastedSpawnItems.Add(cast);
			}
			item->SetPositionInViewport(FVector2D(HorizontalOffset + HorizontalInterval * i, VerticalOffset + VerticalOffset * i));
		}
	}
	
	auto* horizontalBox = Cast<UHorizontalBox>(Panel);
	int characterListIndex = 0;
	int tempIndex = 0;

	if (horizontalBox)
	{
		for (int i = 0; i< SpawnItems.Num(); i++)
		{
			UHorizontalBoxSlot* slot = horizontalBox->AddChildToHorizontalBox(SpawnItems[i]);
			
			slot->SetVerticalAlignment(VAlign_Top);
            slot->SetHorizontalAlignment(HAlign_Center);

			if (auto* cast = Cast<UTurnCharacterList>(FetchedDatas[FMath::Clamp(characterListIndex, 0, FetchedDatas.Num()- 1)]))
			{
				SpawnItems[i]->FetchData(cast->Characters[tempIndex].Character);
				tempIndex++;

				if (tempIndex >= cast->Characters.Num())
				{
					characterListIndex++;
					tempIndex = 0;
				}
			}
		}
	}
}

void UTurnListViewer::MoveCursor(int32 Gap, bool bIsRefresh)
{
	SetCursor(GetCursor() + Gap);

	int characterListIndex = 0;
	int tempIndex = 0;
	
	if (ItemPoolCount > FetchedDatas.Num())
	{
		for (int i = 0; i< SpawnItems.Num(); i++)
		{
			SpawnItems[i]->Deselected();
			
			if (auto* cast = Cast<UTurnCharacterList>(FetchedDatas[FMath::Clamp(characterListIndex, 0, FetchedDatas.Num()- 1)]))
			{
				SpawnItems[i]->FetchData(cast->Characters[FMath::Clamp(tempIndex, 0, cast->Characters.Num() - 1)].Character);
				tempIndex++;

				if (tempIndex >= cast->Characters.Num())
				{
					characterListIndex++;
					tempIndex = 0;
				}
			}
		}

		for (int i = 0; i < SelectedItems.Num(); i++)
		{
			if (!SelectedItems[i]) continue;
			SelectedItems[i]->Selected();
		}
		
		if (bIsRefresh) return;

		if (auto* cast = Cast<UTurnCharacterList>(FetchedDatas[GetCursor()]))
		{
			SelectedTurnList = cast;
		}

		SetSelectedItems();
		for (int i = 0; i < SelectedItems.Num(); i++)
		{
			if (!SelectedItems[i]) continue;
			SelectedItems[i]->Selected();
			if (SelectedItems[i]->GetFetchedCharacter()->GetIsTurn()) continue;

			if (Cast<ANonPlayableCharacterBase>(SelectedItems[i]->GetFetchedCharacter()))
			{
				if (i > 0)
				{
					auto* prevItem = SelectedItems[i - 1]; 
					auto* item = SelectedItems[i];
					TurnHandle = prevItem->GetFetchedCharacter()->OnCharacterTurnEnd.Add(FSimpleDelegate::CreateLambda([this, item, prevItem]()
					{
						item->GetFetchedCharacter()->TurnReceive();
						bool bIsSuccess = prevItem->GetFetchedCharacter()->OnCharacterTurnEnd.Remove(TurnHandle);
					}));
					continue;
				}
			}
			SelectedItems[i]->GetFetchedCharacter()->TurnReceive();
		}
	}
	//Super::MoveCursor(Gap);
}

void UTurnListViewer::RefreshOnDataFetched()
{
	Super::RefreshOnDataFetched();

	TArray<FCharacterTurnData> fetchedCharacters;
	
	for (auto* data : FetchedDatas)
	{
		if (auto* charList = Cast<UTurnCharacterList>(data))
		{
			for (auto& character : charList->Characters)
			{
				fetchedCharacters.Add(character);	
			}
		}
	}

	auto* findItem = CastedSpawnItems.FindByPredicate([fetchedCharacters](const UTurnPortraitItem* i)
	{
		return !fetchedCharacters.ContainsByPredicate([i](const FCharacterTurnData& d)
		{
			return d.Character == i->GetFetchedCharacter();
		});
	});

	if (*findItem)
	{
		int32 itemIndex = SpawnItems.IndexOfByPredicate([findItem](const UViewerItemBase* i)
		{
			return i == (*findItem);
		});
		SpawnItems[itemIndex]->RemoveFromParent();
		SpawnItems.RemoveAt(itemIndex);
		CastedSpawnItems.RemoveAt(itemIndex);
	}
	
	TArray<UTurnCharacterList*> fetchedDataCast;
	
	for (auto* data : FetchedDatas)
	{
		if (auto* cast = Cast<UTurnCharacterList>(data))
		{
			fetchedDataCast.Add(cast);
		}
	}

	int32 newCursor = GetCursorByTurnListPtr(SelectedTurnList, fetchedDataCast);
	if (newCursor >= 0)
	{
		Cursor = newCursor;
	}
	else
	{
		int32 temp = GetCursorByTurnListPtr(SelectedTurnList, CachedList);
		temp++;
		
		while (newCursor >= 0)
		{
			newCursor = GetCursorByTurnListPtr(CachedList[UBGUtil::ClampCursor(temp, CachedList.Num())], fetchedDataCast);
			temp++;
		}

		Cursor = newCursor;
	}

	OnRefreshList.Broadcast();
}

void UTurnListViewer::SetSelectedItems()
{
	SelectedItems.Empty();
	
	if (auto* cast = Cast<UTurnCharacterList>(FetchedDatas[GetCursor()]))
	{
		for (auto& character : cast->Characters)
		{
			if (auto* selected = CastedSpawnItems.FindByPredicate([character](const UTurnPortraitItem* i)
			{
				return i->GetFetchedCharacter() == character.Character;
			}))
			{
				FString name = (*selected)->GetFetchedCharacter()->GetStatus()->GetName();
				UE_LOG(LogTemp, Warning, TEXT("Selected : %s"), *name);
				SelectedItems.Add(*selected);
			}
		}
	}
}

int32 UTurnListViewer::GetCursorByTurnListPtr(class UTurnCharacterList* target, TArray<UTurnCharacterList*> list)
{
	for (int32 i = 0; i < list.Num(); i++)
	{
		if (list[i] == target) return i;
	}

	return -1;
}
