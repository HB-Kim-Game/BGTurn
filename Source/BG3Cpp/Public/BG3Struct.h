// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BG3Enums.h"
#include "BG3Struct.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FObjectStatus : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	// 종족
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGameCharacterRace Race;
	// 직업
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGameCharacterClass CharClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Portrait;
	// 힘
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Str;
	// 민첩
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Dex;
	// 건강
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Con;
	// 지능
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Int;
	// 지혜
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Wis;
	// 매력
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Charisma;
	// 방어력
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 Defensive;
	// 이동력
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MOV;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level;
	// 스펠 갯수
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DefaultSpellOneCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DefaultSpellTwoCount;
};

USTRUCT(Atomic, BlueprintType)
struct FGameAction : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ActionID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EActionCase ActionCase;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AmountTurn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESkillCase SkillCase;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETargetCase TargetCase;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxTargetCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int UpcastNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Texture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TexturePath;
};

USTRUCT(Atomic, BlueprintType)
struct FCharacterTurnData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AMoveCharacterBase* Character;
	UPROPERTY(editAnywhere, BlueprintReadWrite)
	int32 Initiative;
};

USTRUCT(Atomic, BlueprintType)
struct FUIData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* Texture;
	UPROPERTY(editAnywhere, BlueprintReadWrite)
	FString Description;
};
