// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomTimer.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Tickable.h"

void UCustomTimer::SetTimer(UWorld* world, float duration, TFunction<void(float)> OnTick, TFunction<void()> OnFinished)
{
	if (!world) return;

	UCustomTimer* timer = NewObject<UCustomTimer>();
	timer->AddToRoot();
	timer->Initialize(world, duration, MoveTemp(OnTick), MoveTemp(OnFinished));
}

void UCustomTimer::BeginDestroy()
{
	FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
	UObject::BeginDestroy();
}

void UCustomTimer::Initialize(UWorld* world, float duration, TFunction<void(float)> OnTick, TFunction<void()> OnFinished)
{
	WorldPtr = world;
	TickCallback = MoveTemp(OnTick);
	FinishCallback = MoveTemp(OnFinished);
	TotalDuration = duration;

	TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UCustomTimer::Tick));
}

bool UCustomTimer::Tick(float deltaTime)
{
	ElapsedTime += deltaTime;

	if (TickCallback)
	{
		TickCallback(FMath::Clamp(ElapsedTime / TotalDuration, 0.0f, 1.f));
	}

	if (ElapsedTime >= TotalDuration)
	{
		if (FinishCallback)
		{
			FinishCallback();
		}

		FTSTicker::GetCoreTicker().RemoveTicker(TickHandle);
		RemoveFromRoot();
		return false;
	}
	
	return true;
}
