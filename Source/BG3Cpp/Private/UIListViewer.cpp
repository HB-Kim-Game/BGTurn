// Fill out your copyright notice in the Description page of Project Settings.


#include "UIListViewer.h"

#include "BGUtil.h"

void UUIListViewer::MoveCursor(int32 Gap, bool bIsRefresh)
{
	Super::MoveCursor(Gap);
	
	for (int i = 0; i< SpawnItems.Num(); i++)
	{
		SpawnItems[i]->FetchData(FetchedDatas[UBGUtil::ClampCursor(GetCursor() - CursorOffset + i, FetchedDatas.Num())]);
		SpawnItems[i]->Deselected();
	}

	SpawnItems[GetCursor()]->Selected();
}

void UUIListViewer::InitializeItem()
{
	for (int32 i = 0; i < ItemPoolCount; i++)
	{
		UViewerItemBase* item = Cast<UViewerItemBase>(CreateWidget(GetWorld(), ItemClass));
		if (item)
		{
			SpawnItems.Add(item);
			item->SetPositionInViewport(FVector2D(HorizontalOffset + HorizontalInterval * i, VerticalOffset + VerticalOffset * i));
		}
	}
}
