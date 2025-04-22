// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MouseControlledPlayer.generated.h"

UCLASS()
class BG3CPP_API AMouseControlledPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMouseControlledPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	private:
	UFUNCTION()
	void OnLeftMouseButtonDown();

	UPROPERTY()
	float MouseX;
	UPROPERTY()
	float MouseY;
	UPROPERTY()
	FVector MouseWorldPos;
	UPROPERTY()
	FVector MouseWorldDir;
	
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputMappingContext* DefaultContext;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* ClickAction;
};
