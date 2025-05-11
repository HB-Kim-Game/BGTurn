
#include "TurnListViewer.h"

#include "BGUtil.h"
#include "MoveCharacterBase.h"
#include "TurnCharacterList.h"
#include "TurnPortraitItem.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/HorizontalBox.h"

void UTurnListViewer::OnDataFetched()
{
	Super::OnDataFetched();

	UE_LOG(LogTemp, Warning, TEXT("%d"), FetchedDatas.Num());

	MoveCursor(0);
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
				UE_LOG(LogTemp, Warning, TEXT("characterListIndex : %d"), characterListIndex);
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
		
		if (bIsRefresh) return;
		
		for (auto* selectedItem : GetSelectedItems())
		{
			if (!selectedItem) continue;
			selectedItem->Selected();
			if (selectedItem->GetFetchedCharacter()->GetIsTurn()) continue;
			selectedItem->GetFetchedCharacter()->TurnReceive();
		}
	}
	//Super::MoveCursor(Gap);
}

TArray<UTurnPortraitItem*> UTurnListViewer::GetSelectedItems()
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
				SelectedItems.Add(*selected);
			}
		}
	}

	return SelectedItems;
}
