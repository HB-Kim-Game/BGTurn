// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BG3Struct.h"
#include "TurnCharacterList.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UTurnCharacterList : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FCharacterTurnData> Characters;
};
