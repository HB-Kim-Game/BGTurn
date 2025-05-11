// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnPortraitItem.h"

#include "CharacterStatus.h"
#include "MouseControlledPlayer.h"
#include "MoveCharacterBase.h"
#include "PlayableCharacterBase.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"

AMoveCharacterBase* UTurnPortraitItem::GetFetchedCharacter() const
{
	return FetchedCharacter;
}

void UTurnPortraitItem::OnButtonHovered()
{
}

void UTurnPortraitItem::OnButtonUnhovered()
{
}

void UTurnPortraitItem::OnButtonClicked()
{
	if (FetchedCharacter)
	{
		Player->Focus(FVector(FetchedCharacter->GetActorLocation().X, FetchedCharacter->GetActorLocation().Y, Player->GetActorLocation().Z));
		Player->Select(Cast<ISelectableObject>(FetchedCharacter));
	}
}

void UTurnPortraitItem::NativeConstruct()
{
	Super::NativeConstruct();

	Player = Cast<AMouseControlledPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());

	SelectButton->OnClicked.AddDynamic(this, &UTurnPortraitItem::OnButtonClicked);
	SelectButton->OnHovered.AddDynamic(this, &UTurnPortraitItem::OnButtonHovered);
	SelectButton->OnUnhovered.AddDynamic(this, &UTurnPortraitItem::OnButtonUnhovered);

	HpMatDynamic = UMaterialInstanceDynamic::Create(HpMaterial, this);

	Portrait->SetBrushFromMaterial(HpMatDynamic);
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
	SizeBox->SetWidthOverride(110.f);
	SizeBox->SetHeightOverride(150.f);
}

void UTurnPortraitItem::FetchData(UObject* Data)
{
	Super::FetchData(Data);

	if (auto* c = Cast<AMoveCharacterBase>(Data))
	{
		FetchedCharacter = c;
		
		FButtonStyle style = SelectButton->GetStyle();
		
		if (auto* p = Cast<APlayableCharacterBase>(c))
		{
			style.Normal.TintColor = FLinearColor::Blue;
		}
		else style.Normal.TintColor = FLinearColor::Red;
		
		SelectButton->SetStyle(style);
		HpMatDynamic->SetTextureParameterValue("CharacterPortrait", c->Status.Portrait);
		HpMatDynamic->SetScalarParameterValue("Percentage", static_cast<float>(c->GetStatus()->GetHp() - *c->CurHPPtr) / c->GetStatus()->GetHp());
		Portrait->SetDesiredSizeOverride(FVector2D(64.f, 64.f));
	}
	
}
