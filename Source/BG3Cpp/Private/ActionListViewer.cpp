// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionListViewer.h"

#include "BGUtil.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"

void UActionListViewer::OnDataFetched()
{
	Super::OnDataFetched();

	for (auto* item : SpawnItems)
	{
		item->FetchData(nullptr);
	}
	
	MoveCursor(0);
}

void UActionListViewer::InitializeItem()
{
	Super::InitializeItem();

	auto* grid = Cast<UGridPanel>(Panel);

	if (grid)
	{
		for (int i = 0; i < SpawnItems.Num(); i++)
		{
			UGridSlot* slot = grid->AddChildToGrid(SpawnItems[i]);
			
			slot->SetRow(i / grid->ColumnFill.Num());
			slot->SetColumn(i % grid->ColumnFill.Num());
		}
	}

	if (SpawnItems.Num() > FetchedDatas.Num())
	{
		for (int i = 0; i< FetchedDatas.Num(); i++)
		{
			SpawnItems[i]->FetchData(FetchedDatas[UBGUtil::ClampCursor(GetCursor() - CursorOffset + i, FetchedDatas.Num())]);
		}
	}
	else
	{
		for (int i = 0; i< SpawnItems.Num(); i++)
		{
			SpawnItems[i]->FetchData(FetchedDatas[UBGUtil::ClampCursor(GetCursor() - CursorOffset + i, FetchedDatas.Num())]);
		}
	}


}

void UActionListViewer::MoveCursor(int32 Gap)
{
	if (SpawnItems.Num() > FetchedDatas.Num())
	{
		for (int i = 0; i< FetchedDatas.Num(); i++)
		{
			SpawnItems[i]->FetchData(FetchedDatas[UBGUtil::ClampCursor(GetCursor() - CursorOffset + i, FetchedDatas.Num())]);
		}
	}
	else
	{
		for (int i = 0; i< SpawnItems.Num(); i++)
		{
			SpawnItems[i]->FetchData(FetchedDatas[UBGUtil::ClampCursor(GetCursor() - CursorOffset + i, FetchedDatas.Num())]);
		}
	}
}
