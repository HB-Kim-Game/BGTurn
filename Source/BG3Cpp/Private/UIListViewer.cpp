// Fill out your copyright notice in the Description page of Project Settings.


#include "UIListViewer.h"

void UUIListViewer::MoveCursor(int32 Gap)
{
	Super::MoveCursor(Gap);
	for (UViewerItemBase* item : SpawnItems)
	{
		item->Deselected();
	}

	SpawnItems[GetCursor()]->Selected();
}

void UUIListViewer::InitializeItem(TArray<UObject*> datas)
{
	for (int32 i = 0; i < datas.Num(); i++)
	{
		UViewerItemBase* item = Cast<UViewerItemBase>(CreateWidget(GetWorld(), ItemClass));
		if (item)
		{
			Panel->AddChild(item);
			item->FetchData(datas[i]);
			//오프셋 적용 해야함.
			SpawnItems.Add(item);
		}
	}
}
