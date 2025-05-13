// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionTypeDescriptionUI.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UActionTypeDescriptionUI::SetActionType(UTexture2D* texture, FString text)
{
	IconImage->SetBrushFromTexture(texture);
	IconImage->SetDesiredSizeOverride(FVector2D(32.f, 32.f));
	
	DescriptionText->SetText(FText::FromString(text));
}
