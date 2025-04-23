// Fill out your copyright notice in the Description page of Project Settings.


#include "MouseControlledPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "PlayableCharacterBase.h"
#include "SelectableObject.h"

// Sets default values
AMouseControlledPlayer::AMouseControlledPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitializeInput();
}



// Called when the game starts or when spawned
void AMouseControlledPlayer::BeginPlay()
{
	Super::BeginPlay();
	auto* pc = GetWorld()->GetFirstPlayerController();
	FInputModeGameAndUI inputMode = FInputModeGameAndUI();
	inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	inputMode.SetHideCursorDuringCapture(false);
	pc->SetInputMode(inputMode);
	pc->SetShowMouseCursor(true);

	if (UEnhancedInputLocalPlayerSubsystem* sub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer()))
	{
		sub->AddMappingContext(DefaultContext, 0);	
	}
}

// Called every frame
void AMouseControlledPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (nullptr != selectedPlayableChar && *selectedPlayableChar->MovablePtr)
	{
		auto* pc = GetWorld()->GetFirstPlayerController();
		if (pc)
		{
			FHitResult hit;
			if (pc->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_WorldDynamic), false, hit))
			{
				
				FVector dest = FVector(hit.ImpactPoint.X, hit.ImpactPoint.Y, hit.ImpactPoint.Z + selectedPlayableChar->GetActorLocation().Z);
				if (FVector::Distance(dest, lastCursorPos) < 20.f) return;
				lastCursorPos = dest;
				selectedPlayableChar->ShowPath(dest);
				//UE_LOG(LogTemp, Warning, TEXT("Destination : %f, %f, %f"), hit.ImpactPoint.X, hit.ImpactPoint.Y, hit.ImpactPoint.Z + selectedPlayableChar->GetActorLocation().Z);
			}
		}
	}

}

// Called to bind functionality to input
void AMouseControlledPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this, &AMouseControlledPlayer::OnLeftMouseButtonDown);
		EnhancedInputComponent->BindAction(ForwardAction, ETriggerEvent::Triggered, this, &AMouseControlledPlayer::ForwardMove);
		EnhancedInputComponent->BindAction(BackAction, ETriggerEvent::Triggered, this, &AMouseControlledPlayer::BackMove);
		EnhancedInputComponent->BindAction(LeftAction, ETriggerEvent::Triggered, this, &AMouseControlledPlayer::LeftMove);
		EnhancedInputComponent->BindAction(RightAction, ETriggerEvent::Triggered, this, &AMouseControlledPlayer::RightMove);
	}
}

void AMouseControlledPlayer::OnLeftMouseButtonDown()
{
	auto* pc = GetWorld()->GetFirstPlayerController();
	
	if (pc)
	{
		FHitResult hit;
		// TraceChannel - Pawn으로 커서로 누른 지점에 hit 체크
		if (pc->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1), false, hit))
		{
						
			// HitActor가 선택할 수 있는 물체일경우
			// Selected 함수 실행
			// 이후 현재 선택된 오브젝트 변수에 담아둠
			if (auto* actor = Cast<ISelectableObject>(hit.GetActor()))
			{
				if (nullptr != SelectedObject)
				{
					SelectedObject->Deselected();
				}
				
				actor->Selected();
				SelectedObject = actor;

				UE_LOG(LogTemp, Warning, TEXT("selected"));
				if (auto* playable = Cast<APlayableCharacterBase>(actor))
				{
					selectedPlayableChar = playable;
				}
				else
				{
					selectedPlayableChar = nullptr;
				}
			}
			return;
		};

		// 클릭한 곳이 바닥일 경우, 현재 선택한 오브젝트가 존재하고, 그 캐릭터가 move가 가능하다면 클릭한 지점으로 이동

		if (pc->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_WorldDynamic), false, hit))
		{
			if (nullptr == selectedPlayableChar) return;
			if (*(selectedPlayableChar->MovablePtr))
			{
				UE_LOG(LogTemp, Warning, TEXT("Movable"))
				selectedPlayableChar->SetDestination(lastCursorPos);
				//UE_LOG(LogTemp, Warning, TEXT("Destination : %f, %f, %f"), hit.ImpactPoint.X, hit.ImpactPoint.Y, selectedPlayableChar->GetActorLocation().Z);
				selectedPlayableChar->Move();
			}
		}
	}
}

void AMouseControlledPlayer::ForwardMove(const FInputActionValue& value)
{
	if (bIsforward) return;

	FVector2D v = value.Get<FVector2D>();
	
	bIsforward = true;
	Direction.X = v.X;
}

void AMouseControlledPlayer::BackMove(const FInputActionValue& value)
{
	if (bIsback) return;

	FVector2D v = value.Get<FVector2D>();
	
	bIsback = true;
	Direction.X = v.X;
}

void AMouseControlledPlayer::LeftMove(const FInputActionValue& value)
{
	if (bIsLeft) return;

	FVector2D v = value.Get<FVector2D>();
	
	bIsLeft = true;
	Direction.Y = v.Y;
}

void AMouseControlledPlayer::RightMove(const FInputActionValue& value)
{
	if (bIsRight) return;

	FVector2D v = value.Get<FVector2D>();
	
	bIsRight = true;
	Direction.Y = v.Y;
}

void AMouseControlledPlayer::InitializeInput()
{
	ConstructorHelpers::FObjectFinder<UInputMappingContext> defaultContext(TEXT("/Game/BG3/Input/BG3InputMappingContext.BG3InputMappingContext"));

	if (defaultContext.Succeeded())
	{
		DefaultContext = defaultContext.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> clickAction(TEXT("/Game/BG3/Input/Action/IA_LeftClick.IA_LeftClick"));
	if (clickAction.Succeeded())
	{
		ClickAction = clickAction.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> forwardAction(TEXT("/Game/BG3/Input/Action/IA_Forward.IA_Forward"));

	if (forwardAction.Succeeded())
	{
		ForwardAction = forwardAction.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> backAction(TEXT("/Game/BG3/Input/Action/IA_Back.IA_Back"));

	if (backAction.Succeeded())
	{
		BackAction = backAction.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> leftAction(TEXT("/Game/BG3/Input/Action/IA_Left.IA_Left"));

	if (leftAction.Succeeded())
	{
		LeftAction = leftAction.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> rightAction(TEXT("/Game/BG3/Input/Action/IA_Right.IA_Right"));

	if (rightAction.Succeeded())
	{
		RightAction = rightAction.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> cursorMoveAction(TEXT("/Game/BG3/Input/Action/IA_MouseMove.IA_MouseMove"));

	if (cursorMoveAction.Succeeded())
	{
		CursorMoveAction = cursorMoveAction.Object;
	}
}

