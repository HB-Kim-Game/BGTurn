// Fill out your copyright notice in the Description page of Project Settings.


#include "MovableCharacterController.h"

#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

void AMovableCharacterController::Move(FVector Location)
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

	if (NavSys && GetPawn())
	{
		UNavigationPath* path = NavSys->FindPathToLocationSynchronously(
			GetWorld(),
			GetPawn()->GetActorLocation(),
			Location,
			GetPawn());

		UE_LOG(LogTemp, Warning, TEXT("%d"), path->IsValid());
		
		if (path && path->IsValid() && !path->IsPartial())
		{
			UE_LOG(LogTemp, Warning, TEXT("Path Found"));
			const TArray<FVector>& pathPoints = path->PathPoints;

			for (int32 i = 0; i < pathPoints.Num() - 1; ++i)
			{
				DrawDebugLine(
				GetWorld(),
				pathPoints[i],
				pathPoints[i + 1],
				FColor::Green,
				false,
				5.0f,
				0,
				5.0f
			);
			}
		}
	}
	
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
