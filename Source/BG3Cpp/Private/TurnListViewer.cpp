
#include "TurnListViewer.h"

#include "BGUtil.h"
#include "MoveCharacterBase.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/HorizontalBox.h"

void UTurnListViewer::OnDataFetched()
{
	Super::OnDataFetched();

	MoveCursor(0);
}

void UTurnListViewer::InitializeItem()
{

	int32 itemCount = ItemPoolCount >= FetchedDatas.Num() ? FetchedDatas.Num() : ItemPoolCount;
	
	for (int32 i = 0; i < itemCount; i++)
	{
		UViewerItemBase* item = Cast<UViewerItemBase>(CreateWidget(GetWorld(), ItemClass));
		if (item)
		{
			SpawnItems.Add(item);
			item->SetPositionInViewport(FVector2D(HorizontalOffset + HorizontalInterval * i, VerticalOffset + VerticalOffset * i));
		}
	}
	
	auto* horizontalBox = Cast<UHorizontalBox>(Panel);

	if (horizontalBox)
	{
		for (int i = 0; i< SpawnItems.Num(); i++)
		{
			UHorizontalBoxSlot* slot = horizontalBox->AddChildToHorizontalBox(SpawnItems[i]);
			
			slot->SetVerticalAlignment(VAlign_Top);
            slot->SetHorizontalAlignment(HAlign_Center);
			
			SpawnItems[i]->FetchData(FetchedDatas[UBGUtil::ClampCursor(GetCursor() - CursorOffset + i, FetchedDatas.Num())]);
		}
	}
}

void UTurnListViewer::MoveCursor(int32 Gap)
{
	SetCursor(GetCursor() + Gap);
	
	if (ItemPoolCount > FetchedDatas.Num())
	{
		for (int i = 0; i< SpawnItems.Num(); i++)
		{
			SpawnItems[i]->Deselected();
		}

		SpawnItems[GetCursor()]->Selected();
	}

	if (auto* cast = Cast<AMoveCharacterBase>(GetSelectedItem()))
	{
		cast->TurnReceive();
	}
	
	//Super::MoveCursor(Gap);
}
