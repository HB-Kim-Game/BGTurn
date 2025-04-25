// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BGUtil.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UBGUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
#pragma region 캐릭터 가산치 계산 함수
	UFUNCTION(BlueprintCallable, Category = "Math")
	static int32 CalculateBonus(uint8 charStat);
#pragma endregion

	
};
