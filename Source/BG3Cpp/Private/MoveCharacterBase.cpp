// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveCharacterBase.h"

#include "CharacterStatus.h"
#include "InitiativeUI.h"
#include "MovableCharacterController.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "CharacterActionData.h"
#include "BG3GameMode.h"
#include "ActionManager.h"
#include "Kismet/GameplayStatics.h"

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

	Initialize();
}

UNavigationPath* AMoveCharacterBase::ThinkPath(const FVector& dest)
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
		
		FVector ProjectedLocation;
		bool bIsSuccess = UNavigationSystemV1::K2_ProjectPointToNavigation(
			GetWorld(),
			dest,
			ProjectedLocation,
			nullptr,
			nullptr,
			FVector(150, 150, 200));

		if (bIsSuccess)
		{
			UNavigationPath* adjustPath = UNavigationSystemV1::FindPathToLocationSynchronously(
				GetWorld(),
				GetController()->GetPawn()->GetActorLocation(),
				ProjectedLocation,
				GetController()->GetPawn());

			if (adjustPath && adjustPath->IsValid() && !adjustPath->IsPartial())
			{
				if (adjustPath->GetPathLength() / 100.f > CurrentMOV) bIsMovable = false;
				else bIsMovable = true;
				return adjustPath;
			}
		}
	}

	bIsMovable = false;
	return nullptr;
}

void AMoveCharacterBase::Initialize()
{
	SelectedMatDynamic = UMaterialInstanceDynamic::Create(SelectedMaterial, this);
	SpawnDefaultController();

	charController = Cast<AMovableCharacterController>(GetController());
	charController->OnAIMoveCompleted.AddUObject(this, &AMoveCharacterBase::OnMoveCompleted);
	
	OnCharacterPrepareAction.AddLambda([this](){ this-> bIsPrepareAction = true;});
	OnCharacterPrepareBonusAction.AddLambda([this](){ this-> bIsPrepareAction = true;});
	OnCharacterAction.AddLambda([this]()
	{
		this->CurTurnActionCount -= 1;
		this->bIsPrepareAction = false;
	});
	OnCharacterBonusAction.AddLambda([this]()
	{
		this->CurTurnBonusActionCount -= 1;
		this->bIsPrepareAction = false;
	});

	Status = *(DataTable->FindRow<FObjectStatus>(TableName, FString("")));
	CurrentMOV = Status.MOV;

	DetailStatus->Initialize(Status);

	CurHP = DetailStatus->GetHp();

	CurTurnActionCount = 0;
	
	InitiativeUI = Cast<UInitiativeUI>(CreateWidget(GetWorld(), InitiativeClass));
	InitiativeUI->AddToViewport();

	OnInitialized.Broadcast();
}

void AMoveCharacterBase::Move()
{
	auto* path = ThinkPath(Destination);
	if (path->IsValid())
	{
		LastMoveDistance = path->GetPathLength() / 100.f;
		charController->Move(Destination);
		bIsMovable = false;
	}
}

void AMoveCharacterBase::ExecuteAction(ABG3GameMode* mode, UCharacterActionData* action, const FVector& targetLocation)
{
	// 이동후 행동
	TArray<FVector> points;
	const int count = 16;
	const float radius = action->MaxDistance * 100.f - 200.f;
	FVector center = targetLocation;

	for (int i = 0; i < count; i++)
	{
		float Angle = i * (2 * PI / count);
		FVector Offset = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0) * radius;
		points.Add(center + Offset);
	}

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	FVector bestLocation = FVector::ZeroVector;
	float bestDistance = FLT_MAX;

	for (const FVector& point : points)
	{
		FNavLocation projected;
		if (NavSys->ProjectPointToNavigation(point, projected))
		{
			UNavigationPath* p = UNavigationSystemV1::FindPathToLocationSynchronously(GetWorld(), GetActorLocation(), projected.Location);

			if (p && p->IsValid() && !p->IsPartial())
			{
				if (p->GetPathLength() < bestDistance && p->GetPathLength() / 100.f <= CurrentMOV)
				{
					bestDistance = p->GetPathLength();
					bestLocation = projected.Location;
				}
			}
		}
	}

	if (!bestLocation.IsZero())
	{
		if (auto* castController = Cast<AMovableCharacterController>(GetController()))
		{
			FDelegateHandle handle = castController->OnAIMoveCompleted.AddLambda([mode, action, castController, handle, this]()
			{
				mode->ActionManager->ExecuteAction(action, this);
				castController->OnAIMoveCompleted.Remove(handle);
			});
		}
		SetDestination(bestLocation);
		Move();
	}
}

void AMoveCharacterBase::OnMoveCompleted()
{
	bIsMovable = true;
	CurrentMOV -= LastMoveDistance;
}

void AMoveCharacterBase::Selected()
{
	ISelectableObject::Selected();

	SetOutline(true);
}

void AMoveCharacterBase::Deselected()
{
	ISelectableObject::Deselected();

	SetOutline(false);
}

UCharacterStatus* AMoveCharacterBase::GetStatus() const
{
	return DetailStatus;
}

int AMoveCharacterBase::GetCurrentTurnActionCount() const
{
	return CurTurnActionCount;
}

int AMoveCharacterBase::GetCurrentBonusActionCount() const
{
	return CurTurnBonusActionCount;  
}

int AMoveCharacterBase::GetMaxTurnActionCount() const
{
	return MaxTurnActionCount;
}

int AMoveCharacterBase::GetMaxBonusActionCount() const
{
	return MaxTurnBonusActionCount;
}

bool AMoveCharacterBase::GetIsTurn() const
{
	return bIsTurn;
}

bool AMoveCharacterBase::GetIsPrepare() const
{
	return bIsPrepareAction;
}

void AMoveCharacterBase::TurnReceive()
{
	bIsTurn = true;
	CurrentMOV = Status.MOV;
	CurTurnActionCount = 1;
	CurTurnBonusActionCount = 1;
	bIsMovable = true;
	
	OnCharacterTurnReceive.Broadcast();
}

void AMoveCharacterBase::TurnEnd()
{
	bIsTurn = false;
	bIsMovable = false;
	OnCharacterTurnEnd.Broadcast();
}

void AMoveCharacterBase::SetOutline(bool condition)
{
	if (condition)
	{
		GetMesh()->SetOverlayMaterial(SelectedMatDynamic);
	}
	else
	{
		GetMesh()->SetOverlayMaterial(nullptr);
	}
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

void AMoveCharacterBase::SetDestination(const FVector& location)
{
	Destination = location;
}

void AMoveCharacterBase::ShowInitiative(int32 num)
{
	FVector worldLocation = GetActorLocation() + FVector(0, 0, 200.f);
	
	InitiativeUI->ShowInitiative(worldLocation, num);
}


