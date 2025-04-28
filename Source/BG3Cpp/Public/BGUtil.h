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

#pragma region 리스트 Clamp 함수, Max는 exclude
	UFUNCTION(BlueprintCallable, Category = "Math")
	static int32 ClampCursor(int32 value, int32 max, int min = 0);
#pragma endregion 
};
