// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SelectableObject.h"
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


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetDestination(FVector location);

	virtual void Move();

	UFUNCTION()
	void OnMoveCompleted();
	
public:
	const bool* MovablePtr;

	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterialInterface* SelectedMaterial;

	UPROPERTY()
	class UMaterialInstanceDynamic* SelectedMatDynamic;

	UPROPERTY()
	FVector Destination;
	
private:
	bool bIsMovable = true;

	class AMovableCharacterController* controller;
};
