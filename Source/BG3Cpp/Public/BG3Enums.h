// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BG3Enums.generated.h"

UENUM(BlueprintType)
enum class EGameMouseState : uint8
{
	Default UMETA(DisplayName = "Default"),
	Move UMETA(DisplayName = "Move"),
	ActionHover UMETA(DisplayName = "ActionHover"),
};

UENUM(BlueprintType)
enum class EGameCharacterClass : uint8
{
	Fighter UMETA(DisplayName = "Fighter"),
	Barbarian UMETA(DisplayName = "Barbarian"),
	Cleric UMETA(DisplayName = "Cleric"),
	Druid UMETA(DisplayName = "Druid"),
	Paladin UMETA(DisplayName = "Paladin"),
	Ranger UMETA(DisplayName = "Ranger"),
	Monk UMETA(DisplayName = "Monk"),
	Rogue UMETA(DisplayName = "Rogue"),
	Sorcerer UMETA(DisplayName = "Sorcerer"),
	Wizard UMETA(DisplayName = "Wizard"),
	Warlock UMETA(DisplayName = "Warlock"),
};

UENUM(BlueprintType)
enum class EGameCharacterRace : uint8
{
	Human UMETA(DisplayName = "Human"),
	Elf UMETA(DisplayName = "Elf"),
	Dwarf UMETA(DisplayName = "Dwarf"),
	Halfling UMETA(DisplayName = "Halfling"),
	Gnome UMETA(DisplayName = "Gnome"),
	HalfElf UMETA(DisplayName = "HalfElf"),
	HalfOrk UMETA(DisplayName = "HalfOrk"),
	Dragonborn UMETA(DisplayName = "Dragonborn"),
	Tiefling UMETA(DisplayName = "Tiefling"),
};

UENUM(BlueprintType)
enum class EDiceResult : uint8
{
	Fail UMETA(DisplayName = "Fail"),
	Success UMETA(DisplayName = "Success"),
	Critical UMETA(DisplayName = "Critical"),
};

UENUM(BlueprintType)
enum class EActionCase : uint8
{
	DefaultAction UMETA(DisplayName = "DefaultAction"),
	BonusAction UMETA(DisplayName = "BonusAction"),
};