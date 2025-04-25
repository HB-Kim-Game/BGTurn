// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SelectableObject.h"
#include "BG3Struct.h"
#include "GameFramework/Character.h"
#include "MoveCharacterBase.generated.h"

UCLASS()
class BG3CPP_API AMoveCharacterBase : public ACharacter, public ISelectableObject
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMoveCharacterBase();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	class UNavigationPath* ThinkPath(FVector dest);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetDestination(FVector location);

	virtual void Move();

	UFUNCTION()
	virtual void OnMoveCompleted();

	class UCharacterStatus* GetStatus();
	
public:
	const bool* MovablePtr;

	const int32* CurHPPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FObjectStatus Status;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TableName;
	
protected:

	class UDataTable* DataTable;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterialInterface* SelectedMaterial;

	UPROPERTY()
	class UMaterialInstanceDynamic* SelectedMatDynamic;

	UPROPERTY()
	FVector Destination;

	float LastMoveDistance;
	float CurrentMOV;
	
	bool bIsMovable = true;
	int32 CurHP;
	
	class AMovableCharacterController* controller;
	
	UPROPERTY(EditAnywhere)
	class UCharacterStatus* DisplayStatus;
};
