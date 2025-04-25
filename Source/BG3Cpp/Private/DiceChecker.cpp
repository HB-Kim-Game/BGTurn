// Fill out your copyright notice in the Description page of Project Settings.


#include "DiceChecker.h"

#include "BGUtil.h"

EDiceResult UDiceChecker::CheckDiceResult(uint8 charStat, uint8 targetNum, int8 diceNum)
{
	int diceResult = RollDice(diceNum);

	// 모조건 실패
	if (diceResult <= 1) return EDiceResult::Fail;
	// 대성공
	if (diceResult >= 20) return EDiceResult::Critical;
	
	return diceResult + UBGUtil::CalculateBonus(charStat) >= targetNum ? EDiceResult::Success : EDiceResult::Fail;
}

int8 UDiceChecker::RollDice(int8 diceNum)
{
	return FMath::RandRange(1, diceNum);
}

