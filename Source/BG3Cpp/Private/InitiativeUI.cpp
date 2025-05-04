// Fill out your copyright notice in the Description page of Project Settings.


#include "InitiativeUI.h"

#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UInitiativeUI::ShowInitiative(FVector location, int32 initiativeNum)
{
	SetVisibility(ESlateVisibility::Visible);
	StopAllAnimations();
	PlayAnimation(Appear);
	InitiativeNum->SetText(FText::FromString(FString::FromInt(initiativeNum)));
	
	if (DisappearHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(DisappearHandle);
	}

	GetWorld()->GetTimerManager().SetTimer(DisappearHandle, [this]()
	{
		this->StopAllAnimations();
		this->PlayAnimation(Disappear);
	}, 1.5f, false);

	if (PositionHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(PositionHandle);
	}

	auto* pc = this->GetWorld()->GetFirstPlayerController();
	FVector2D screenLocation;
	UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), location, screenLocation);
		
	if (pc && pc->ProjectWorldLocationToScreen(location, screenLocation))
	{
		this->SetPositionInViewport(screenLocation, true);
	}

	GetWorld()->GetTimerManager().SetTimer(PositionHandle, [this, location]()
	{
		auto* pc = this->GetWorld()->GetFirstPlayerController();
		FVector2D screenLocation;
		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), location, screenLocation);
		
		if (pc && pc->ProjectWorldLocationToScreen(location, screenLocation))
		{
			this->SetPositionInViewport(screenLocation, true);
		}
		
	}, 0.05f, true);

}

void UInitiativeUI::NativeConstruct()
{
	Super::NativeConstruct();
	SetVisibility(ESlateVisibility::Collapsed);

	DisappearEvent.BindDynamic(this, &UInitiativeUI::DisappearEnd);
	BindToAnimationFinished(Disappear, DisappearEvent);

	SetAlignmentInViewport(FVector2D(0.5f, 1.0f));
}

void UInitiativeUI::DisappearEnd()
{
	SetVisibility(ESlateVisibility::Collapsed);
	if (PositionHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(PositionHandle);
	}
}
