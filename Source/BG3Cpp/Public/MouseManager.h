// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BG3Enums.h"
#include "MouseManager.generated.h"


UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BG3CPP_API UMouseManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMouseManager();

	void Initialize(EGameMouseState mouseState = EGameMouseState::Default);
	void SetMouseMode(EGameMouseState mouseState);

	class UCursorBase* GetCursor();

	const EGameMouseState* MouseStatePtr;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<class UCursorBase>> CursorClasses;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UCursorBase*> Cursors;

	class UCursorBase* CurrentCursor;
	EGameMouseState MouseState;
	
	void CreateCursors();
};
