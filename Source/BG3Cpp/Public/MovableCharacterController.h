// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MovableCharacterController.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE(FOnAIMoveCompletedDelegate);

UCLASS()
class BG3CPP_API AMovableCharacterController : public AAIController
{
	GENERATED_BODY()

public:
	void Move(const FVector& Location);
	FOnAIMoveCompletedDelegate OnAIMoveCompleted;
	
protected:
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
};
