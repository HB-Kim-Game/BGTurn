// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionHoverCursor.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UActionHoverCursor::ShowActionDetail(FGameAction* action)
{
	
	DisplayName->SetText(FText::FromString(action->DisplayName));
	DescriptionText->SetText(FText::FromString(action->DisplayName));

	FSlateBrush temp;
	if (action->Texture)
	{
		temp.SetResourceObject(action->Texture);
		temp.SetImageSize(FVector2D(32.f, 32.f));
		IconImage->SetBrush(temp);	
	}
}
