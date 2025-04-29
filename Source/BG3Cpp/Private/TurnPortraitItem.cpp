// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnPortraitItem.h"

#include "MoveCharacterBase.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"

void UTurnPortraitItem::OnButtonHovered()
{
}

void UTurnPortraitItem::OnButtonUnhovered()
{
}

void UTurnPortraitItem::OnButtonClicked()
{
}

void UTurnPortraitItem::NativeConstruct()
{
	Super::NativeConstruct();
}

void UTurnPortraitItem::Selected()
{
	Super::Selected();
	
	SizeBox->SetWidthOverride(130.f);
	SizeBox->SetHeightOverride(170.f);
}

void UTurnPortraitItem::Deselected()
{
	Super::Deselected();
	SizeBox->SetWidthOverride(100.f);
	SizeBox->SetHeightOverride(140.f);
}

void UTurnPortraitItem::FetchData(UObject* Data)
{
	Super::FetchData(Data);

	if (auto* c = Cast<AMoveCharacterBase>(Data))
	{
		Portrait->SetBrushFromTexture(c->Status.Portrait);
		Portrait->SetDesiredSizeOverride(FVector2D(64.f, 64.f));
	}
	
}
