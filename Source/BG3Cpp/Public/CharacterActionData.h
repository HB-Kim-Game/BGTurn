// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BG3Enums.h"
#include "UObject/NoExportTypes.h"
#include "CharacterActionData.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UCharacterActionData : public UObject
{
	GENERATED_BODY()

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
	class AMoveCharacterBase* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Texture;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TexturePath;
};
