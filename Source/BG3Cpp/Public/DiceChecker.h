// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BG3Enums.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DiceChecker.generated.h"

UCLASS()
class BG3CPP_API UDiceChecker : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static EDiceResult CheckDiceResult(uint8 charStat, uint8 targetNum, int8 diceNum = 20);

	static int8 RollDice(int8 diceNum = 20);
};
