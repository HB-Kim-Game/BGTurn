// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIListViewer.h"
#include "TurnListViewer.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UTurnListViewer : public UUIListViewer
{
	GENERATED_BODY()

	virtual void OnDataFetched() override;
	
	virtual void InitializeItem() override;

	virtual void MoveCursor(int32 Gap) override;
};
