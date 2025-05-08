// Fill out your copyright notice in the Description page of Project Settings.


#include "BGUtil.h"

int32 UBGUtil::CalculateBonus(uint8 charStat)
{
	return (charStat - 10) / 2;
}

int32 UBGUtil::ClampCursor(int32 value, int32 max, int min)
{
	if (value >= max)
	{
		while (value >= max) value -= max;
	}

	if (value < min)
	{
		while (value < min) value += max;
	}

	return value;
}

int32 UBGUtil::CalculateFallingDamage(int32 maxHp, float fallingDistance)
{
	int32 damageResult = maxHp * (fallingDistance / 100.f - 4.f) / 17.f + 1;

	return damageResult;
}
