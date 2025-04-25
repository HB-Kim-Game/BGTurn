// Fill out your copyright notice in the Description page of Project Settings.


#include "BGUtil.h"

int32 UBGUtil::CalculateBonus(uint8 charStat)
{
	return (charStat - 10) / 2;
}
