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
	virtual void PrepareAction(class AMoveCharacterBase* character, class UCharacterActionData* action);
	virtual void ExecuteAction(class AMoveCharacterBase* character, class UCharacterActionData* action);
	
};

UCLASS()
class BG3CPP_API UMeleeAction : public UActionBase
{
	GENERATED_BODY()

public:
	
	FDelegateHandle ExecuteActionHandle;
	
	virtual void PrepareAction(class AMoveCharacterBase* character, class UCharacterActionData* action) override;
	virtual void ExecuteAction(class AMoveCharacterBase* character, class UCharacterActionData* action) override;
};

UCLASS()
class BG3CPP_API USprintAction : public UActionBase
{
	GENERATED_BODY()

public:
	virtual void PrepareAction(class AMoveCharacterBase* character, class UCharacterActionData* action) override;
	virtual void ExecuteAction(class AMoveCharacterBase* character, class UCharacterActionData* action) override;
};

UCLASS()
class BG3CPP_API UJumpAction : public UActionBase
{
	GENERATED_BODY()

public:
	virtual void PrepareAction(class AMoveCharacterBase* character, class UCharacterActionData* action) override;
	virtual void ExecuteAction(class AMoveCharacterBase* character, class UCharacterActionData* action) override;
};

UCLASS()
class BG3CPP_API UFireBallAction : public UActionBase
{
	GENERATED_BODY()

public:
	virtual void PrepareAction(class AMoveCharacterBase* character, class UCharacterActionData* action) override;
	virtual void ExecuteAction(class AMoveCharacterBase* character, class UCharacterActionData* action) override;
};

UCLASS()
class BG3CPP_API URangedAttackAction : public UActionBase
{
	GENERATED_BODY()

public:
	virtual void PrepareAction(class AMoveCharacterBase* character, class UCharacterActionData* action) override;
	virtual void ExecuteAction(class AMoveCharacterBase* character, class UCharacterActionData* action) override;
};