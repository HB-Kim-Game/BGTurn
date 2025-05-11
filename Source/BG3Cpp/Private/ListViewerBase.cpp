// Fill out your copyright notice in the Description page of Project Settings.


#include "ListViewerBase.h"
#include "BGUtil.h"

int32 UListViewerBase::GetCursor() const
{
	return Cursor;
}

void UListViewerBase::SetCursor(int32 value)
{
	Cursor = UBGUtil::ClampCursor(value, FetchedDatas.Num());
}

UObject* UListViewerBase::GetSelectedItem() const
{
	return FetchedDatas[Cursor];
}

void UListViewerBase::MoveCursor(int32 Gap, bool bIsRefresh)
{
	SetCursor(Cursor + Gap);
}