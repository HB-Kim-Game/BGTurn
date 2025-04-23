// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
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
	void OnLeftMouseButtonDown();
	
	void ForwardMove(const FInputActionValue& value);
	void BackMove(const FInputActionValue& value);
	void LeftMove(const FInputActionValue& value);
	void RightMove(const FInputActionValue& value);
	
	UFUNCTION()
	void InitializeInput();
	
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputMappingContext* DefaultContext;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* ClickAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* ForwardAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* BackAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* LeftAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* RightAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* CursorMoveAction;

	class ISelectableObject* SelectedObject;

	bool bIsforward = false;
	bool bIsback = false;
	bool bIsLeft = false;
	bool bIsRight = false;
	
	class APlayableCharacterBase* selectedPlayableChar;

	FVector Direction;

	FVector lastCursorPos;
};
