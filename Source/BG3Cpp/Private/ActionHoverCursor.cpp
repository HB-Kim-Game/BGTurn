// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionHoverCursor.h"

#include "CharacterActionData.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"

void UActionHoverCursor::ShowActionDetail(UCharacterActionData* action)
{
	DisplayName->SetText(FText::FromString(action->DisplayName));
	FString lineBreakText = "<br>";
	FString lineBreak = "\r\n";
	FString result = action->Description.Replace(*lineBreakText, *lineBreak);
	DescriptionText->SetText(FText::FromString(result));

	FSlateBrush temp;
	if (action->Texture)
	{
		temp.SetResourceObject(action->Texture);
		temp.SetImageSize(FVector2D(100.f, 100.f));
		IconImage->SetBrush(temp);	
	}
}
