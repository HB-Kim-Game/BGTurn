// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ParabolaSpline.generated.h"

UCLASS()
class BG3CPP_API AParabolaSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AParabolaSpline();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitializeParabola(float pointValue, float height);
	FVector CalculateParabola(const FVector& startPosition, float alpha) const;
	float SetTarget(const FVector& targetLocation);

	void SetbIsExecute(bool condition);

	float GetLength() const;

	float GetHeight() const;

	bool GetIsBlocked() const;

	FVector GetDestination() const;

private:
	UPROPERTY()
	class USplineComponent* Spline;
	UPROPERTY()
	class UStaticMesh* SplineMeshInstance;
	UPROPERTY()
	class UMaterial* SplineMaterial;

	UPROPERTY()
	class USceneComponent* Root;

	UPROPERTY()
	TArray<class USplineMeshComponent*> SplineMeshes;
	
	bool bIsExecute;

	void AddSplineMesh();
	void RemoveSplineMesh();

	FVector Direction;
	FVector lastCursorPos;
	FVector Destination = FVector::ZeroVector;
	float PointValue;
	float DefaultHeight;
	float Height;

	bool bInitialized = false;
	bool bIsBlocked = false;

	float SplineLength;

	UPROPERTY()
	class AAttackRange* PointDecal;

	UPROPERTY()
	class APlayerController* pc;
};
