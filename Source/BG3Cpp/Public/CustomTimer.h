// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CustomTimer.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UCustomTimer : public UObject
{
	GENERATED_BODY()

public:
	static void SetTimer(UWorld* world, float duration, TFunction<void(float)> OnTick, TFunction<void()> OnFinished = nullptr);

	virtual void BeginDestroy() override;

private:
	void Initialize(UWorld* world, float duration, TFunction<void(float)> OnTick, TFunction<void()> OnFinished = nullptr);
	bool Tick(float deltaTime);

	TWeakObjectPtr<UWorld> WorldPtr;
	TFunction<void(float)> TickCallback;
	TFunction<void()> FinishCallback;

	float ElapsedTime = 0.0f;
	float TotalDuration = 0.0f;

	FTSTicker::FDelegateHandle TickHandle;
};
