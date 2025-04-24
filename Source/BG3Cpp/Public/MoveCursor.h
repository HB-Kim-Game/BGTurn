// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CursorBase.h"
#include "MoveCursor.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UMoveCursor : public UCursorBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Distance;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WarningText;

	void ShowDistance(float distance, bool isMovable);
};
