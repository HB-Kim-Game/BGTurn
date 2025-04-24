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
	
	float ShowPath(FVector dest);

	virtual void OnMoveCompleted() override;

	float GetCurrentMOV();

	bool GetIsMoving();

private:
	class USplineComponent* Spline;
	class UStaticMesh* SplineMeshInstance;
	class UMaterial* SplineMaterial;

	bool bIsMoving = false;

	TArray<class USplineMeshComponent*> SplineMeshes;

	void AddSplineMesh();
	void RemoveSplineMesh();
};
