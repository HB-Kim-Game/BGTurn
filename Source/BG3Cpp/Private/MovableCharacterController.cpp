// Fill out your copyright notice in the Description page of Project Settings.


#include "MovableCharacterController.h"
#include "Navigation/PathFollowingComponent.h"

void AMovableCharacterController::Move(FVector Location)
{
	MoveToLocation(Location);
}

void AMovableCharacterController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (Result.IsSuccess())
	{
		OnAIMoveCompleted.Broadcast();
		UE_LOG(LogTemp, Warning, TEXT("MoveComplete"));
	}
	
}
