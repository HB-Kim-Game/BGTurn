// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "BG3Enums.h"
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

	class UMouseManager* GetMouseManager() const;

	class APlayableCharacterBase* GetPlayableCharacter() const;

	class UPlayerUI* GetPlayerUI() const;

	void Focus(FVector focusLocation);

	void Select(class ISelectableObject* selectedObject);

	void SetFocusEnemy(class ANonPlayableCharacterBase* character);
	
	private:
	void OnLeftMouseButtonDown();
	void OnAltTriggered();
	void OnAltComplete();
	void OnRightMouseButtonDown();

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
#pragma region Camera Movement
	void StartForwardMove(const FInputActionValue& value);
	void StartBackMove(const FInputActionValue& value);
	void StartLeftMove(const FInputActionValue& value);
	void StartRightMove(const FInputActionValue& value);

	void StartRotateRight(const FInputActionValue& value);
	void StartRotateLeft(const FInputActionValue& value);
	
	void ForwardMove(const FInputActionValue& value);
	void BackMove(const FInputActionValue& value);
	void LeftMove(const FInputActionValue& value);
	void RightMove(const FInputActionValue& value);

	void RotateRight(const FInputActionValue& value);
	void RotateLeft(const FInputActionValue& value);
	
	void StopForwardMove(const FInputActionValue& value);
	void StopBackMove(const FInputActionValue& value);
	void StopLeftMove(const FInputActionValue& value);
	void StopRightMove(const FInputActionValue& value);

	void StopRotateRight(const FInputActionValue& value);
	void StopRotateLeft(const FInputActionValue& value);

	void Wheel(const FInputActionValue& value);
	
	UFUNCTION()
	void InitializeInput();

	UPROPERTY(EditAnywhere, Category = Input)
	float MoveSpeed = 500.f;

	UPROPERTY(EditAnywhere, Category = Input)
	float WheelSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = Input)
	float MinLength = 450.f;

	UPROPERTY(EditAnywhere, Category = Input)
	float MaxLength = 2400.f;

	UPROPERTY()
	float TargetLength = MaxLength;
	
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputMappingContext* DefaultContext;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* ClickAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* RightClickAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* OutlineAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* ForwardAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* BackAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* LeftAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* RightAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* UpAction;
	
	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* DownAction;

	UPROPERTY(VisibleAnywhere, Category = Input)
	class UInputAction* WheelAction;
	
#pragma  endregion 
	
	class ISelectableObject* SelectedObject;


	bool bIsforward = false;
	bool bIsback = false;
	bool bIsLeft = false;
	bool bIsRight = false;
	bool bIsUp = false;
	bool bIsDown = false;

	bool bIsFocus = false;
	bool bIsOutline = false;

	UPROPERTY()
	class APlayableCharacterBase* selectedPlayableChar;
	UPROPERTY()
	class AMoveCharacterBase* hoverCharacter;

	UPROPERTY()
	class ANonPlayableCharacterBase* SelectedEnemyCharacter;

	FVector ForwardDirection;
	FVector RightDirection;
	
	FVector Direction;

	FVector lastCursorPos;

	FVector FocusPoint;

	UPROPERTY(EditAnywhere, Category = Cursor)
	TSubclassOf<class UMouseManager> MouseManagerClass;
	
	UPROPERTY()
	class UMouseManager* MouseManager;
	
	EGameMouseState MouseState;
	
	UPROPERTY(EditAnywhere, Category = UI)
	TSubclassOf<class UPlayerUI> PlayerUIClass;

	UPROPERTY()
	class UPlayerUI* PlayerUI;

	UPROPERTY(EditAnywhere)
	class USceneComponent* Root;

	UPROPERTY(EditAnywhere)
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* CameraComp;

};
