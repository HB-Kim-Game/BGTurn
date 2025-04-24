// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveCursor.h"

#include "Components/TextBlock.h"

void UMoveCursor::ShowDistance(float distance, bool isMovable)
{
	FString temp = FString::Printf(TEXT("%.1f"), distance / 100.f);
	temp.Append("m");
	Distance->SetText(FText::FromString(temp));

	WarningText->SetVisibility(isMovable ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
}
