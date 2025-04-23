// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableCharacterBase.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

APlayableCharacterBase::APlayableCharacterBase()
{
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
	Spline->ClearSplinePoints();
	Spline->SetupAttachment(RootComponent);
	
	SplineMeshes.Reserve(15);

	ConstructorHelpers::FObjectFinder<UStaticMesh> splineMesh(TEXT("/Game/BG3/Model/SplinePlane.SplinePlane"));
	ConstructorHelpers::FObjectFinder<UMaterial> material(TEXT("/Game/BG3/Model/M_Spline.M_Spline"));

	if (splineMesh.Succeeded())
	{
		SplineMeshInstance = splineMesh.Object;	
	}

	if (material.Succeeded())
	{
		SplineMaterial = material.Object;
	}
}

void APlayableCharacterBase::Selected()
{
	Super::Selected();

	GetMesh()->SetOverlayMaterial(SelectedMatDynamic);
}

void APlayableCharacterBase::Deselected()
{
	Super::Deselected();

	GetMesh()->SetOverlayMaterial(nullptr);
	Spline->ClearSplinePoints();
	for (USplineMeshComponent* mesh : SplineMeshes)
	{
		mesh->DestroyComponent();
	}
	SplineMeshes.Empty(15);
}

void APlayableCharacterBase::Move()
{
	Super::Move();
}

void APlayableCharacterBase::ShowPath(FVector dest)
{
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

	if (NavSys && GetController()->GetPawn())
	{
		UNavigationPath* path = NavSys->FindPathToLocationSynchronously(
			GetWorld(),
			GetController()->GetPawn()->GetActorLocation(),
			dest,
			GetController()->GetPawn());
		
		if (path && path->IsValid() && !path->IsPartial())
		{
			UE_LOG(LogTemp, Warning, TEXT("%f"), path->GetPathLength());
			const TArray<FVector>& pathPoints = path->PathPoints;

			Spline->ClearSplinePoints();
			// 시작점
			Spline->AddSplinePoint(FVector(GetActorLocation().X, GetActorLocation().Y, pathPoints[0].Z + 5.f), ESplineCoordinateSpace::World);
			
			for (FVector point : pathPoints)
			{
				Spline->AddSplinePoint(FVector(point.X, point.Y, point.Z + 5.f), ESplineCoordinateSpace::World);
			}

			AddSplineMesh();
		}
	}
}

void APlayableCharacterBase::AddSplineMesh()
{
	for (USplineMeshComponent* mesh : SplineMeshes)
	{
		mesh->DestroyComponent();
	}
	SplineMeshes.Empty(15);

	for (int i = 0; i < Spline->GetNumberOfSplinePoints() - 1; i++)
	{
		USplineMeshComponent* temp = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());

		if (nullptr != temp)
		{
			temp->SetStaticMesh(SplineMeshInstance);
			temp->Mobility = EComponentMobility::Type::Movable;
			temp->RegisterComponent();
			temp->AttachToComponent(Spline, FAttachmentTransformRules::KeepRelativeTransform);
		}

		FVector pointLocation1;
		FVector pointTangent1;
		Spline->GetLocationAndTangentAtSplinePoint(i, pointLocation1, pointTangent1, ESplineCoordinateSpace::Local);

		FVector pointLocation2;
		FVector pointTangent2;
		Spline->GetLocationAndTangentAtSplinePoint(i + 1, pointLocation2, pointTangent2, ESplineCoordinateSpace::Local);

		temp->SetStartAndEnd(pointLocation1, pointTangent1, pointLocation2, pointTangent2, true);

		SplineMeshes.Add(temp);

		temp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}
}
