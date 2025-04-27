// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveCharacterBase.h"

#include "CharacterStatus.h"
#include "MovableCharacterController.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AMoveCharacterBase::AMoveCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MovablePtr = &bIsMovable;
	CurHPPtr = &CurHP;

	ConstructorHelpers::FObjectFinder<UDataTable> dataTable(TEXT("/Game/BG3/DataTable/GameCharacterDataTable.GameCharacterDataTable"));
	if (dataTable.Succeeded())
	{
		DataTable = dataTable.Object;
	}

	GetCapsuleComponent()->SetCanEverAffectNavigation(true);
	GetCapsuleComponent()->bDynamicObstacle = true;

	DetailStatus = CreateDefaultSubobject<UCharacterStatus>(TEXT("DisplayStatus"));
}

// Called when the game starts or when spawned
void AMoveCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	SelectedMatDynamic = UMaterialInstanceDynamic::Create(SelectedMaterial, this);
	SpawnDefaultController();

	controller = Cast<AMovableCharacterController>(GetController());
	controller->OnAIMoveCompleted.AddDynamic(this, &AMoveCharacterBase::OnMoveCompleted);

	OnCharacterAction.AddLambda([this](){this->TurnActionCount -= 1;});
	OnCharacterBonusAction.AddLambda([this](){this->TurnBonusActionCount -= 1;});

	Status = *(DataTable->FindRow<FObjectStatus>(TableName, FString("")));
	CurrentMOV = Status.MOV;

	DetailStatus->Initialize(Status);

	CurHP = DetailStatus->GetHp();
}

UNavigationPath* AMoveCharacterBase::ThinkPath(FVector dest)
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
			if (path->GetPathLength() / 100.f > CurrentMOV) bIsMovable = false;
			else bIsMovable = true;
			return path;
		}
	}

	bIsMovable = false;
	return nullptr;
}

void AMoveCharacterBase::Move()
{
	LastMoveDistance = ThinkPath(Destination)->GetPathLength() / 100.f;
	controller->Move(Destination);
	bIsMovable = false;
}

void AMoveCharacterBase::OnMoveCompleted()
{
	bIsMovable = true;
	CurrentMOV -= LastMoveDistance;
}

UCharacterStatus* AMoveCharacterBase::GetStatus()
{
	return DetailStatus;
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


