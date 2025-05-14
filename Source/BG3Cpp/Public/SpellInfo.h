// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpellInfo.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API USpellInfo : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	class UImage* IconImage;
	UPROPERTY(meta = (BindWidget))
	class UButton* Spell1Button;
	UPROPERTY(meta = (BindWidget))
	class UButton* Spell2Button;

	UPROPERTY(meta = (BindWidget))
	class UImage* TargetProgress;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ProjectileNum;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ProjectileName;
};
