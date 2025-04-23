// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveCharacterBase.h"

#include "MovableCharacterController.h"
#include "Components/CapsuleComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Runtime/AIModule/Classes/AIController.h"

// Sets default values
AMoveCharacterBase::AMoveCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MovablePtr = &bIsMovable;

	GetCapsuleComponent()->SetCanEverAffectNavigation(true);
	GetCapsuleComponent()->bDynamicObstacle = true;
}

// Called when the game starts or when spawned
void AMoveCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	SelectedMatDynamic = UMaterialInstanceDynamic::Create(SelectedMaterial, this);
	SpawnDefaultController();

	controller = Cast<AMovableCharacterController>(GetController());
	controller->OnAIMoveCompleted.AddDynamic(this, &AMoveCharacterBase::OnMoveCompleted);
}

void AMoveCharacterBase::Move()
{
	controller->Move(Destination);
	bIsMovable = false;
}

void AMoveCharacterBase::OnMoveCompleted()
{
	bIsMovable = true;
}

// Called every frame
void AMoveCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMoveCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

void AMoveCharacterBase::SetDestination(FVector location)
{
	Destination = location;
}


