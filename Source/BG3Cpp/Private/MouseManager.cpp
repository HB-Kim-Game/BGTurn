// Fill out your copyright notice in the Description page of Project Settings.


#include "MouseManager.h"

#include "CursorBase.h"
#include "Blueprint/UserWidget.h"

// Sets default values for this component's properties
UMouseManager::UMouseManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UMouseManager::Initialize(EGameMouseState mouseState)
{
	CreateCursors();
	SetMouseMode(mouseState);
}

void UMouseManager::SetMouseMode(EGameMouseState mouseState)
{
	MouseState = mouseState;
	CurrentCursor = Cursors[static_cast<uint8>(mouseState)];
	GetWorld()->GetFirstPlayerController()->SetMouseCursorWidget(EMouseCursor::Type::Default, CurrentCursor);
}


// Called when the game starts
void UMouseManager::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UMouseManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMouseManager::CreateCursors()
{
	Cursors.Reserve(10);
	
	for (auto cursor : CursorClasses)
	{
		auto c = Cast<UCursorBase>(CreateWidget(GetWorld(), cursor));
		Cursors.Add(c);
		UE_LOG(LogTemp, Warning, TEXT("Add Cursor"));
	}
}

class UCursorBase* UMouseManager::GetCursor()
{
	return CurrentCursor;
}

