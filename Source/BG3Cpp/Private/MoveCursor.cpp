// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveCursor.h"

#include "Components/TextBlock.h"

void UMoveCursor::ShowDistance(float distance, bool isMovable)
{
	FString temp = FString::Printf(TEXT("%.1f"), distance / 100.f);
	temp.Append("m");
	Distance->SetText(FText::FromString(temp));

	if (distance < 0.01f)
	{
		FString text = TEXT("목표 지점에 도달할 수 없습니다.");
		WarningText->SetText(FText::FromString(text));
	}
	else
	{
		FString text = TEXT("이동력 부족");
		WarningText->SetText(FText::FromString(text));	
	}
	
	WarningText->SetVisibility(isMovable ? ESlateVisibility::Collapsed: ESlateVisibility::Visible);
}
