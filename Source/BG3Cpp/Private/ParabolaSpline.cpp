// Fill out your copyright notice in the Description page of Project Settings.


#include "ParabolaSpline.h"

#include "AttackRange.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

// Sets default values
AParabolaSpline::AParabolaSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	PrimaryActorTick.bCanEverTick = true;
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

// Called when the game starts or when spawned
void AParabolaSpline::BeginPlay()
{
	Super::BeginPlay();

	pc = GetWorld()->GetFirstPlayerController();
}

// Called every frame
void AParabolaSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bInitialized) return;
	
	if (pc)
	{
		FHitResult hit;

		if (pc->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_WorldDynamic), false, hit))
		{

			if (FVector::Distance(hit.ImpactPoint, lastCursorPos) < 20.f) return;
			lastCursorPos = hit.ImpactPoint;
			
			SplineLength = SetTarget(hit.ImpactPoint);
		}
	}
}

void AParabolaSpline::InitializeParabola(float pointValue, float height)
{
	PointValue = pointValue;
	Height = height;
	DefaultHeight = height;
	
	// 도약 지점 데칼 생성
	// 캐릭터 주변으로 범위 표시
	PointDecal = GetWorld()->SpawnActor<AAttackRange>(
		AAttackRange::StaticClass(),
		GetActorLocation(),
		FRotator(0, 0, 0));

	PointDecal->SetDecalRange(1.f);

	PointDecal->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));

	bInitialized = true;
}

FVector AParabolaSpline::CalculateParabola(const FVector& startPosition, float alpha) const
{
	FVector location = FMath::Lerp(startPosition, GetDestination(), alpha)
					+ (FVector::UpVector * FMath::Sin(alpha * PI) * Height);

	return location;
}

float AParabolaSpline::SetTarget(const FVector& targetLocation)
{
	Spline->ClearSplinePoints();

	Spline->AddSplinePoint(FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z), ESplineCoordinateSpace::World);

	Direction = (targetLocation - GetActorLocation()).GetSafeNormal();

	Height = DefaultHeight + FVector::DistXY(GetActorLocation(), targetLocation) / 450.f * 100.f;
	
	for (float alpha = 0.f; alpha <= 1.f; alpha += PointValue)
	{
		FVector location = FMath::Lerp(GetActorLocation(), targetLocation, alpha)
							+ (FVector::UpVector * FMath::Sin(alpha * PI) * Height);
		
		Spline->AddSplinePoint(location, ESplineCoordinateSpace::World);
	}

	FVector last = targetLocation + (FVector::UpVector * FMath::Sin(PI) * Height);

	PointDecal->SetActorLocation(last);
		
	Spline->AddSplinePoint(last, ESplineCoordinateSpace::World);

	AddSplineMesh();

	return Spline->GetSplineLength();
}

float AParabolaSpline::GetLength() const
{
	if (Spline->GetNumberOfSplinePoints() > 0) return Spline->GetSplineLength();

	return 0.f;
}

FVector AParabolaSpline::GetDestination() const
{
	if (Spline->GetNumberOfSplinePoints() > 0) return Spline->GetLocationAtSplinePoint(Spline->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);

	return FVector::ZeroVector;
}

void AParabolaSpline::AddSplineMesh()
{
	RemoveSplineMesh();
	
	for (int i = 0; i < Spline->GetNumberOfSplinePoints() - 1; i++)
	{
		USplineMeshComponent* temp = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());

		if (nullptr != temp)
		{
			temp->SetStartScale(FVector2d(1.0f,0.05f));
			temp->SetEndScale(FVector2d(1.0f,0.05f));
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
	}
}

void AParabolaSpline::RemoveSplineMesh()
{
	for (USplineMeshComponent* mesh : SplineMeshes)
	{
		mesh->DestroyComponent();
	}
	SplineMeshes.Empty(15);
}

