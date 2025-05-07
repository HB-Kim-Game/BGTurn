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

int32 UCharacterStatus::GetHp() const
{
	return HP;
}

FString UCharacterStatus::GetName() const
{
	return FileName;
}

TArray<FGameAction*> UCharacterStatus::GetActions() const
{
	return Actions;
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
		CharLevel = Status.Level;
		// HP 주사위 굴려서 생성하고 집어넣기
		HP = CreateHPwithDice();
		CreateActions();
		
		Save();
	}

	if (Actions.Num() <= 0) CreateActions();
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
		break;
	case EGameCharacterClass::Cleric :
		diceNum = 8;
		break;
	case EGameCharacterClass::Rogue :
		diceNum = 8;
		break;
	case EGameCharacterClass::Druid :
		diceNum = 8;
		break;
	case EGameCharacterClass::Paladin :
		diceNum = 10;
		break;
	case EGameCharacterClass::Warlock :
		diceNum = 6;
		break;
	case EGameCharacterClass::Sorcerer :
		diceNum = 6;
		break;
	case EGameCharacterClass::Barbarian :
		diceNum = 12;
		break;
	case EGameCharacterClass::Ranger :
		diceNum = 10;
		break;
	}

	int result = 0;
	for (int i = 1; i <= CharLevel; i++)
	{
		result += UDiceChecker::RollDice(diceNum) + UBGUtil::CalculateBonus(Status.Con);
	}
	
	return result;
}

void UCharacterStatus::CreateActions()
{
	FString actionTablePath = "";
	
	switch (Status.CharClass)
	{
	case EGameCharacterClass::Fighter :
		actionTablePath = "/Game/BG3/DataTable/FighterActionTable.FighterActionTable";
		break;
	case EGameCharacterClass::Monk :
		actionTablePath = "/Game/BG3/DataTable/MonkActionTable.MonkActionTable";
		break;
	case EGameCharacterClass::Wizard :
		actionTablePath = "/Game/BG3/DataTable/WizardActionTable.WizardActionTable";
		break;
	case EGameCharacterClass::Cleric :
		actionTablePath = "/Game/BG3/DataTable/ClericActionTable.ClericActionTable";
		break;
	case EGameCharacterClass::Rogue :
		actionTablePath = "/Game/BG3/DataTable/RogueActionTable.RogueActionTable";
		break;
	case EGameCharacterClass::Druid :
		actionTablePath = "/Game/BG3/DataTable/DruidActionTable.DruidActionTable";
		break;
	case EGameCharacterClass::Paladin :
		actionTablePath = "/Game/BG3/DataTable/PaladinActionTable.PaladinActionTable";
		break;
	case EGameCharacterClass::Warlock :
		actionTablePath = "/Game/BG3/DataTable/WarlockActionTable.WarlockActionTable";
		break;
	case EGameCharacterClass::Sorcerer :
		actionTablePath = "/Game/BG3/DataTable/SorcererActionTable.SorcererActionTable";
		break;
	case EGameCharacterClass::Barbarian :
		actionTablePath = "/Game/BG3/DataTable/BarbarianActionTable.BarbarianActionTable";
		break;
	case EGameCharacterClass::Ranger :
		actionTablePath = "/Game/BG3/DataTable/RangerActionTable.RangerActionTable";
		break;
	}

	UDataTable* actionTable = LoadObject<UDataTable>(nullptr, *actionTablePath, nullptr, LOAD_None, nullptr);

	if (actionTable)
	{
		actionTable->GetAllRows<FGameAction>(TEXT("Actions"), Actions);
	}
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

		for (FGameAction* action : Actions)
		{
			saveData->SaveActions.Add(*action);
		}

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

		for (const FGameAction& action : loadData->SaveActions)
		{
			FGameAction* saveAction = new FGameAction(action);
			Actions.Add(saveAction);
		}

		return true;
	}

	return false;
}

