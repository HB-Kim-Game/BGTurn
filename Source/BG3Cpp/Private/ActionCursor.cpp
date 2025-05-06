// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionCursor.h"

#include "CharacterActionData.h"
#include "MouseControlledPlayer.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"

UCharacterActionData* UActionCursor::GetAction() const
{
	return Action;
}

void UActionCursor::ShowActionDescription(UCharacterActionData* action, int percent,const FString& description)
{
	IconImage->SetBrushFromTexture(action->Texture);
	Action = action;
	
	if (percent > 90)
	{
		FString text = "<ActionPercentHigh>" + FString::FromInt(percent);
		Percent->SetText(FText::FromString(text));
	}
	else
	{
		if (percent <= 0)
		{
			Percent->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			Percent->SetVisibility(ESlateVisibility::Visible);
			FString text = "<ActionPercent>" + FString::FromInt(percent);
			Percent->SetText(FText::FromString(text));		
		}
	}

	if (description.IsEmpty())
	{
		Description->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		FString text = "<CursorDesc>" + description + "</CursorDesc>";
		Description->SetText(FText::FromString(text));
		Description->SetVisibility(ESlateVisibility::Visible);
	}
	
}

void UActionCursor::NativeConstruct()
{
	Super::NativeConstruct();

	player = Cast<AMouseControlledPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

void UActionCursor::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}
