// Fill out your copyright notice in the Description page of Project Settings.


#include "NonPlayableCharacterBase.h"

#include "BattleTurnManager.h"
#include "BG3GameMode.h"
#include "MovableCharacterController.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "PlayableCharacterBase.h"
#include "Components/CapsuleComponent.h"

void ANonPlayableCharacterBase::Selected()
{
	Super::Selected();

	//GetMesh()->SetOverlayMaterial(SelectedMatDynamic);
}

void ANonPlayableCharacterBase::Deselected()
{
	Super::Deselected();

	//GetMesh()->SetOverlayMaterial(nullptr);
}

void ANonPlayableCharacterBase::ThinkAction()
{
	EnemyCharacters.Empty();
	
	if (Mode)
	{
		UNavigationPath* path = nullptr;
		
		for (auto c  :Mode->BattleManager->Characters)
		{
			if (Cast<APlayableCharacterBase>(c.Character))
			{
				EnemyCharacters.Add(c.Character);
			}
		}

		FVector bestLocation = FVector::ZeroVector;
		float bestDistance = FLT_MAX;

		for (auto* e : EnemyCharacters)
		{
			FVector center = e->GetActorLocation();
			float radius = e->GetCapsuleComponent()->GetScaledCapsuleRadius() + 75.f;
			TArray<FVector> points;
			for (int i = 0; i < 12; i++)
			{
				float Angle = i * (2 * PI / 12);
				FVector Offset = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0) * radius;
				points.Add(center + Offset);
			}
			UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

			for (const FVector& point : points)
			{
				FNavLocation projected;
				if (NavSys->ProjectPointToNavigation(point, projected))
				{
					UNavigationPath* p = UNavigationSystemV1::FindPathToLocationSynchronously(GetWorld(), GetActorLocation(), projected.Location);

					if (p && p->IsValid() && !p->IsPartial())
					{
						if (p->GetPathLength() < bestDistance)
						{
							bestDistance = p->GetPathLength();
							bestLocation = projected.Location;
						}
					}
				}
			}

			UNavigationPath* tempPath = ThinkPath(bestLocation);
			
			if (path == nullptr)
			{
				path = tempPath;
			}
			else if (tempPath && tempPath->GetPathLength() < path->GetPathLength())
			{
				path = tempPath;
			}
		}

		if (path && path->PathPoints.Num() >= 2)
		{
			if (path->GetPathLength() < GetCapsuleComponent()->GetScaledCapsuleRadius() + 20.f)
			{
				TurnEnd();
				return;
			}
			
			float distanceResult = 0.0f;

			if (auto* cast = Cast<AMovableCharacterController>(GetController()))
			{
				MoveDelegateHandle = cast->OnAIMoveCompleted.Add(FSimpleDelegate::CreateLambda([this, cast]()
				{
					this->TurnEnd();
					bool bIsSuccess = cast->OnAIMoveCompleted.Remove(MoveDelegateHandle);
				}));
			}

			for (int i = 1; i < path->PathPoints.Num(); ++i)
			{
				const FVector& prevPoint = path->PathPoints[i - 1];
				const FVector& currentPoint = path->PathPoints[i];
				float segLength = FVector::Dist(prevPoint, currentPoint);

				if ((distanceResult + segLength) / 100.f > CurrentMOV)
				{
					float remain = CurrentMOV - distanceResult;
					float alpha = remain / segLength;
					SetDestination(FMath::Lerp(prevPoint, currentPoint, alpha));
					Move();
					return;
				}

				distanceResult += segLength;
			}
			
			SetDestination(path->PathPoints.Last());
			Move();
		}
		else
		{
			TurnEnd();
		}
	}
	else
	{
		TurnEnd();
	}
}

void ANonPlayableCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	Initialize();

	Mode = Cast<ABG3GameMode>(GetWorld()->GetAuthGameMode());
}
