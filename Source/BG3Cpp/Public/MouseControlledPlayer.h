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

	private:
	void OnLeftMouseButtonDown();
#pragma region Camera Movement
	void StartForwardMove(const FInputActionValue& value);
	void StartBackMove(const FInputActionValue& value);
	void StartLeftMove(const FInputActionValue& value);
	void StartRightMove(const FInputActionValue& value);

	void StartUpMove(const FInputActionValue& value);
	void StartDownMove(const FInputActionValue& value);
	
	void ForwardMove(const FInputActionValue& value);
	void BackMove(const FInputActionValue& value);
	void LeftMove(const FInputActionValue& value);
	void RightMove(const FInputActionValue& value);

	void UpMove(const FInputActionValue& value);
	void DownMove(const FInputActionValue& value);
	
	void StopForwardMove(const FInputActionValue& value);
	void StopBackMove(const FInputActionValue& value);
	void StopLeftMove(const FInputActionValue& value);
	void StopRightMove(const FInputActionValue& value);

	void StopUpMove(const FInputActionValue& value);
	void StopDownMove(const FInputActionValue& value);
	
	UFUNCTION()
	void InitializeInput();

	UPROPERTY(EditAnywhere, Category = Input)
	float MoveSpeed = 500.f;

	UPROPERTY(EditAnywhere, Category = Input)
	float MinZ = 300.f;

	UPROPERTY(EditAnywhere, Category = Input)
	float MaxZ = 1200.f;
	
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
	class UInputAction* UpAction;
	
	UPROPERTY(VisibleAnywhere, Category = Input)
    	class UInputAction* DownAction;
	
#pragma  endregion 

	class ISelectableObject* SelectedObject;

	bool bIsforward = false;
	bool bIsback = false;
	bool bIsLeft = false;
	bool bIsRight = false;
	bool bIsUp = false;
	bool bIsDown = false;
	
	class APlayableCharacterBase* selectedPlayableChar;

	FVector Direction;

	FVector lastCursorPos;

	UPROPERTY(EditAnywhere, Category = Cursor)
	TSubclassOf<class UMouseManager> MouseManagerClass;

	UPROPERTY()
	class UMouseManager* MouseManager;
	
	EGameMouseState MouseState;
	
	UPROPERTY(EditAnywhere, Category = UI)
	TSubclassOf<class UPlayerUI> PlayerUIClass;

	UPROPERTY()
	class UPlayerUI* PlayerUI;
};
