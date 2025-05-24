// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageUI.h"

#include "CustomTimer.h"
#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UDamageUI::ShowDamage(FVector location, int32 damage)
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
	StopAllAnimations();
	PlayAnimation(Appear);

	UnbindAllFromAnimationFinished(Appear);
	BindToAnimationFinished(Appear, FOnFinished);

	if (damage < 0)
	{
		DamageText->SetText(FText::FromString(TEXT("빗나감!")));	
	}
	else
	{
		DamageText->SetText(FText::FromString(FString::FromInt(damage)));
	}
	
	if (PositionHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(PositionHandle);
	}

	auto* pc = this->GetWorld()->GetFirstPlayerController();
	FVector2D screenLocation;
	UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), location, screenLocation);

	FVector offsetLocation = FVector(location.X, location.Y, location.Z + 120.f);
	
	if (pc && pc->ProjectWorldLocationToScreen(offsetLocation, screenLocation))
	{
		this->SetPositionInViewport(screenLocation, true);
	}
	
	GetWorld()->GetTimerManager().SetTimer(PositionHandle, [this, offsetLocation]()
	{
		auto* pc = this->GetWorld()->GetFirstPlayerController();
		FVector2D screenLocation;
		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), offsetLocation, screenLocation);
		
		if (pc && pc->ProjectWorldLocationToScreen(offsetLocation, screenLocation))
		{
			this->SetPositionInViewport(screenLocation, true);
		}
		
	}, 0.03f, true);
}

void UDamageUI::OnFinished()
{
	if (PositionHandle.IsValid())
	{
		isPlaying = false;
		GetWorld()->GetTimerManager().ClearTimer(PositionHandle);
		SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UDamageUI::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);

	FOnFinished.BindDynamic(this, &UDamageUI::OnFinished);
}
