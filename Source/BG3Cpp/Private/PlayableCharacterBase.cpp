// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableCharacterBase.h"

#include "CharacterActionData.h"
#include "CharacterStatus.h"
#include "InitiativeUI.h"
#include "MouseControlledPlayer.h"
#include "MovableCharacterController.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Blueprint/UserWidget.h"
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
	
}

void APlayableCharacterBase::Deselected()
{
	Super::Deselected();

	Spline->ClearSplinePoints();
	RemoveSplineMesh();
}

void APlayableCharacterBase::Move()
{
	Super::Move();
	bIsMoving = true;
}

TArray<UCharacterActionData*> APlayableCharacterBase::GetActions() const
{
	return Actions;
}

void APlayableCharacterBase::OnMoveCompleted()
{
	Super::OnMoveCompleted();
	RemoveSplineMesh();
	bIsMoving = false;
}

void APlayableCharacterBase::Initialize()
{
	Super::Initialize();

	for (auto action : DetailStatus->GetActions())
	{
		UCharacterActionData* data = NewObject<UCharacterActionData>();
		data->ActionID = action->ActionID;
		data->ActionCase = action->ActionCase;
		data->MaxDistance = action->MaxDistance;
		data->MinDistance = action->MinDistance;
		data->SkillCase = action->SkillCase;
		data->AmountTurn = action->AmountTurn;
		data->DisplayName = action->DisplayName;
		data->Description = action->Description;
		data->Texture = action->Texture;
		data->TexturePath = action->TexturePath;

		Actions.Add(data);
	}
}

float APlayableCharacterBase::GetCurrentMOV() const
{
	return CurrentMOV;
}

bool APlayableCharacterBase::GetIsMoving() const
{
	return bIsMoving;
}

bool APlayableCharacterBase::GetSplineCondition() const
{
	return SplineCondition;
}

void APlayableCharacterBase::SetSplineCondition(bool condition)
{
	SplineCondition = condition;

	if (!SplineCondition)
	{
		Spline->ClearSplinePoints();
		RemoveSplineMesh();
	}
}

void APlayableCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void APlayableCharacterBase::OnMontageEnded(class UAnimMontage* Montage, bool bInterrupted)
{
	Super::OnMontageEnded(Montage, bInterrupted);
}

float APlayableCharacterBase::ShowPath(const FVector& dest, const FVector& extent)
{
	UNavigationPath* path = ThinkPath(dest, extent);

	if (nullptr == path)
	{
		Spline->ClearSplinePoints();
		RemoveSplineMesh();
		return 0.0f;
	}
	
	const TArray<FVector>& pathPoints = path->PathPoints;

	Spline->ClearSplinePoints();
	// 시작점
	Spline->AddSplinePoint(FVector(GetActorLocation().X, GetActorLocation().Y, pathPoints[0].Z + 5.f), ESplineCoordinateSpace::World);
			
	for (FVector point : pathPoints)
	{
		Spline->AddSplinePoint(FVector(point.X, point.Y, point.Z + 5.f), ESplineCoordinateSpace::World);
	}

	AddSplineMesh();

	if (!path->IsValid())
	{
		Spline->ClearSplinePoints();
		RemoveSplineMesh();
		return 0.0f;	
	}

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
