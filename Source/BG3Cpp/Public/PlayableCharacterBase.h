// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MoveCharacterBase.h"
#include "PlayableCharacterBase.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API APlayableCharacterBase : public AMoveCharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayableCharacterBase();

public:
	virtual void Selected() override;
	virtual void Deselected() override;
	virtual void Move() override;

	TArray<class UCharacterActionData*> GetActions() const;
	
	float ShowPath(const FVector& dest, const FVector& extent = FVector(150.f, 150.f, 200.f));

	virtual void OnMoveCompleted() override;

	virtual void Initialize() override;
	
	float GetCurrentMOV() const;

	bool GetIsMoving() const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	class USplineComponent* Spline;
	UPROPERTY()
	class UStaticMesh* SplineMeshInstance;
	UPROPERTY()
	class UMaterial* SplineMaterial;

	UPROPERTY()
	TArray<class UCharacterActionData*> Actions;
	
	bool bIsMoving = false;

	UPROPERTY()
	TArray<class USplineMeshComponent*> SplineMeshes;

	void AddSplineMesh();
	void RemoveSplineMesh();
};
