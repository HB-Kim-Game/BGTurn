// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterStatus.h"

#include "BGUtil.h"
#include "DiceChecker.h"
#include "SaveCharacterData.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UCharacterStatus::UCharacterStatus()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

int32 UCharacterStatus::GetHp()
{
	return HP;
}

FString UCharacterStatus::GetName()
{
	return FileName;
}


// Called when the game starts
void UCharacterStatus::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCharacterStatus::Initialize(FObjectStatus status)
{
	Status = status;
	
	if (!Load())
	{
		CharLevel = status.Level;
		// HP 주사위 굴려서 생성하고 집어넣기
		HP = CreateHPwithDice();
		Save();
	}
}


// Called every frame
void UCharacterStatus::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

int32 UCharacterStatus::CreateHPwithDice()
{
	int8 diceNum = 0;
	switch (Status.CharClass)
	{
	case EGameCharacterClass::Fighter :
		diceNum = 10;
		break;
	case EGameCharacterClass::Monk :
		diceNum = 8;
		break;
	case EGameCharacterClass::Wizard :
		diceNum = 6;
	case EGameCharacterClass::Cleric :
		diceNum = 8;
	case EGameCharacterClass::Rogue :
		diceNum = 8;
	case EGameCharacterClass::Druid :
		diceNum = 8;
	case EGameCharacterClass::Paladin :
		diceNum = 10;
	case EGameCharacterClass::Warlock :
		diceNum = 6;
	case EGameCharacterClass::Sorcerer :
		diceNum = 6;
	case EGameCharacterClass::Barbarian :
		diceNum = 12;
	case EGameCharacterClass::Ranger :
		diceNum = 10;
	}

	int result = 0;
	for (int i = 1; i <= CharLevel; i++)
	{
		result += UDiceChecker::RollDice(diceNum) + UBGUtil::CalculateBonus(Status.Con);
	}
	
	return result;
}

void UCharacterStatus::Save()
{
	USaveCharacterData* saveData = Cast<USaveCharacterData>(UGameplayStatics::CreateSaveGameObject(USaveCharacterData::StaticClass()));

	if (saveData)
	{
		saveData->SaveSlotName = FileName;
		saveData->SaveIndex = 0;

		saveData->SaveDisplayName = FileName;
		saveData->SaveHp = HP;
		saveData->SaveCharacterLevel = CharLevel;

		UGameplayStatics::SaveGameToSlot(saveData, saveData->SaveSlotName, saveData->SaveIndex);
	}
}

bool UCharacterStatus::Load()
{
	
	USaveCharacterData* loadData = Cast<USaveCharacterData>(UGameplayStatics::LoadGameFromSlot(FileName, 0));

	if (loadData)
	{
		HP = loadData->SaveHp;
		CharLevel = loadData->SaveCharacterLevel;

		return true;
	}

	return false;
}

