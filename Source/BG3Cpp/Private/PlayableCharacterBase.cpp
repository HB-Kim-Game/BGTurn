// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableCharacterBase.h"

#include "MovableCharacterController.h"
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

	//GetMesh()->SetOverlayMaterial(SelectedMatDynamic);
}

void APlayableCharacterBase::Deselected()
{
	Super::Deselected();

	//GetMesh()->SetOverlayMaterial(nullptr);
	Spline->ClearSplinePoints();
	RemoveSplineMesh();
}

void APlayableCharacterBase::Move()
{
	Super::Move();
	bIsMoving = true;
}

void APlayableCharacterBase::OnMoveCompleted()
{
	Super::OnMoveCompleted();
	RemoveSplineMesh();
	bIsMoving = false;
}

float APlayableCharacterBase::GetCurrentMOV()
{
	return CurrentMOV;
}

bool APlayableCharacterBase::GetIsMoving()
{
	return bIsMoving;
}

float APlayableCharacterBase::ShowPath(FVector dest)
{
	UNavigationPath* path = ThinkPath(dest);

	if (nullptr == path) return 0.0f;
	
	const TArray<FVector>& pathPoints = path->PathPoints;

	Spline->ClearSplinePoints();
	// 시작점
	Spline->AddSplinePoint(FVector(GetActorLocation().X, GetActorLocation().Y, pathPoints[0].Z + 5.f), ESplineCoordinateSpace::World);
			
	for (FVector point : pathPoints)
	{
		Spline->AddSplinePoint(FVector(point.X, point.Y, point.Z + 5.f), ESplineCoordinateSpace::World);
	}

	AddSplineMesh();

	return path->GetPathLength();
}

void APlayableCharacterBase::AddSplineMesh()
{
	RemoveSplineMesh();
	
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

void APlayableCharacterBase::RemoveSplineMesh()
{
	for (USplineMeshComponent* mesh : SplineMeshes)
	{
		mesh->DestroyComponent();
	}
	SplineMeshes.Empty(15);
}
