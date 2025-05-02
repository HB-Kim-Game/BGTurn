// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ActionBase.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class BG3CPP_API UActionBase : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void PrepareAction(class AMoveCharacterBase* character, float distance);
	virtual void ExecuteAction(class AMoveCharacterBase* character);
	
};

UCLASS()
class BG3CPP_API UMeleeAction : public UActionBase
{
	GENERATED_BODY()

public:
	virtual void PrepareAction(class AMoveCharacterBase* character, float distance) override;
	virtual void ExecuteAction(class AMoveCharacterBase* character) override;
};

UCLASS()
class BG3CPP_API USprintAction : public UActionBase
{
	GENERATED_BODY()

public:
	virtual void PrepareAction(class AMoveCharacterBase* character, float distance) override;
	virtual void ExecuteAction(class AMoveCharacterBase* character) override;
};

UCLASS()
class BG3CPP_API UFireBallAction : public UActionBase
{
	GENERATED_BODY()

public:
	virtual void PrepareAction(class AMoveCharacterBase* character, float distance) override;
	virtual void ExecuteAction(class AMoveCharacterBase* character) override;
};