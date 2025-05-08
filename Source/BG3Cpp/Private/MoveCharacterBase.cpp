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
#include "DamageUI.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	GetCharacterMovement()->bOrientRotationToMovement = true;
}

// Called when the game starts or when spawned
void AMoveCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

UNavigationPath* AMoveCharacterBase::ThinkPath(const FVector& dest, const FVector& extent)
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
			extent);

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

float AMoveCharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	float damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	CurHP = FMath::Clamp(CurHP- damage, 0, GetStatus()->GetHp());

	FVector dir = EventInstigator->GetPawn()->GetActorLocation() - GetActorLocation();
	dir.Z = 0;
	FRotator rotation = dir.Rotation();
	this->SetActorRotation(rotation);

	OnTakeDefaultDamage.Broadcast(DamageAmount, this, Cast<AMoveCharacterBase>(EventInstigator->GetPawn()));	
	if (CurHP > 0)
	{
		PlayAnimation(TEXT("TakeDamage"));
	}
	else
	{
		OnDead.Broadcast();
	}
	
	return damage;
}

void AMoveCharacterBase::Initialize()
{
	OnInitialized.Broadcast();

	SelectedMatDynamic = UMaterialInstanceDynamic::Create(SelectedMaterial, this);
	SpawnDefaultController();

	charController = Cast<AMovableCharacterController>(GetController());
	charController->OnAIMoveCompleted.AddUObject(this, &AMoveCharacterBase::OnMoveCompleted);
	
	OnCharacterPrepareAction.AddLambda([this]()
	{
		this-> bIsPrepareAction = true;
	});
	OnCharacterPrepareBonusAction.AddLambda([this](){ this-> bIsPrepareAction = true;});
	OnCharacterAction.Add(FSimpleDelegate::CreateLambda([this]()
	{
		bIsPrepareAction = false;
		this->CurTurnActionCount -= 1;
	}));
	OnCharacterBonusAction.Add(FSimpleDelegate::CreateLambda([this]()
	{
		bIsPrepareAction = false;
		this->CurTurnBonusActionCount -= 1;
	}));
	OnDead.Add(FSimpleDelegate::CreateLambda([this]()
	{
		this->GetMesh()->PlayAnimation(DeadAnimation, false);
		this->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		this->GetCapsuleComponent()->bDynamicObstacle = false;
		this->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}));

	Status = *(DataTable->FindRow<FObjectStatus>(TableName, FString("")));
	CurrentMOV = Status.MOV;
	MaxMOV = Status.MOV;

	DetailStatus->Initialize(Status);

	CurHP = DetailStatus->GetHp();

	CurTurnActionCount = 0;
	
	InitiativeUI = Cast<UInitiativeUI>(CreateWidget(GetWorld(), InitiativeClass));
	InitiativeUI->AddToViewport();

	DamageUI = Cast<UDamageUI>(CreateWidget(GetWorld(), DamageUIClass));
	DamageUI->AddToViewport();

	GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &AMoveCharacterBase::OnMontageEnded);
}

void AMoveCharacterBase::Move()
{
	auto* path = ThinkPath(Destination);
	if (path->IsValid())
	{
		LastMoveDistance = path->GetPathLength() / 100.f;
		charController->Move(Destination);
		GetMesh()->PlayAnimation(MoveAnimation, true);
		bIsMovable = false;
		CurrentMOV -= LastMoveDistance;
	}
}

void AMoveCharacterBase::ExecuteAction(ABG3GameMode* mode, UCharacterActionData* action, const FVector& targetLocation)
{
	// 이동후 행동
	TArray<FVector> points;
	const int count = 16;
	const float radius = action->MaxDistance * 100.f;
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
			ExecuteActionHandle = castController->OnAIMoveCompleted.Add(FSimpleDelegate::CreateLambda([mode, action, castController, this, targetLocation]()
			{
				FVector dir = targetLocation - this->GetActorLocation();
				dir.Z = 0;
				FRotator rotation = dir.Rotation();
				this->SetActorRotation(rotation);
				mode->ActionManager->ExecuteAction(action, this);
				bool removeSuccess = castController->OnAIMoveCompleted.Remove(this->ExecuteActionHandle);
			}));
		}
		SetDestination(bestLocation);
		
		Move();
	}
}

void AMoveCharacterBase::StopAction()
{
	// 공격범위 제거
	// 마우스 커서 변경
	// 애니메이션 idle 변경
	// 사용 코스트 표시 취소
}

void AMoveCharacterBase::AddMOV(float value, bool isSprint)
{
	CurrentMOV += value;
	if (isSprint)
	{
		MaxMOV += value;
	}
}

void AMoveCharacterBase::OnMoveCompleted()
{
	bIsMovable = true;
	if (!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		GetMesh()->PlayAnimation(IdleAnimation,true);
	}
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

UDamageUI* AMoveCharacterBase::GetDamageUI() const
{
	return DamageUI;
}

float AMoveCharacterBase::GetMaxMov() const
{
	return MaxMOV;
}

void AMoveCharacterBase::TurnReceive()
{
	bIsTurn = true;
	CurrentMOV = MaxMOV;
	CurTurnActionCount = MaxTurnActionCount;
	CurTurnBonusActionCount = MaxTurnBonusActionCount;
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

void AMoveCharacterBase::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsMovable = true;
	GetMesh()->PlayAnimation(IdleAnimation, true);
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

void AMoveCharacterBase::PlayAnimation(const FString& actionID)
{
	if (auto* animation = AnimationMap.Find(actionID))
	{
		if (auto* sequence = Cast<UAnimSequence>(*animation))
		{
			GetMesh()->PlayAnimation(sequence, true);
		}
		else if (auto* montage = Cast<UAnimMontage>(*animation))
		{
			UE_LOG(LogTemp, Warning, TEXT("montage"));
			GetMesh()->SetAnimationMode(EAnimationMode::Type::AnimationBlueprint);
			PlayAnimMontage(montage);
			bIsMovable = false;
		}
		
	}
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

void AMoveCharacterBase::SetIsMovable(bool condition)
{
	bIsMovable = condition;
}


