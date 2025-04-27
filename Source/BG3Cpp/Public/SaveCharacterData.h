// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BG3Struct.h"
#include "GameFramework/SaveGame.h"
#include "SaveCharacterData.generated.h"

UCLASS()
class BG3CPP_API USaveCharacterData : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SaveData")
	FString SaveSlotName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SaveData")
	int32 SaveIndex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SaveData")
	int32 SaveHp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SaveData")
	FString SaveDisplayName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SaveData")
	int32 SaveCharacterLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SaveData")
	TArray<FGameAction> SaveActions;
	
};
