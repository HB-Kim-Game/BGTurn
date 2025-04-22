// Fill out your copyright notice in the Description page of Project Settings.


#include "MouseControlledPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

// Sets default values
AMouseControlledPlayer::AMouseControlledPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UInputMappingContext> defaultContext(TEXT("/Game/BG3/Input/BG3InputMappingContext.BG3InputMappingContext"));

	if (defaultContext.Succeeded())
	{
		DefaultContext = defaultContext.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> clickAction(TEXT("/Game/BG3/Input/LeftMouseClick.LeftMouseClick"));
	if (clickAction.Succeeded())
	{
		ClickAction = clickAction.Object;
	}
}

// Called when the game starts or when spawned
void AMouseControlledPlayer::BeginPlay()
{
	Super::BeginPlay();
	auto* pc = GetWorld()->GetFirstPlayerController();
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

}

// Called to bind functionality to input
void AMouseControlledPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Triggered, this, &AMouseControlledPlayer::OnLeftMouseButtonDown);
	}
}

void AMouseControlledPlayer::OnLeftMouseButtonDown()
{
	auto* pc = GetWorld()->GetFirstPlayerController();

	if (pc)
	{
		pc->GetMousePosition(MouseX, MouseY);
		pc->DeprojectScreenPositionToWorld(MouseX, MouseY, MouseWorldPos, MouseWorldDir);

		UE_LOG(LogTemp, Warning, TEXT("MouseX: %f, MouseY: %f"), MouseX, MouseY);
		UE_LOG(LogTemp, Warning, TEXT("MouseX: %f, MouseY: %f"), MouseWorldPos.X, MouseWorldPos.Y);
	}
}

