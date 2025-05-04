// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SelectableObject.h"
#include "BG3Struct.h"
#include "GameFramework/Character.h"
#include "MoveCharacterBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnCharacterAction);
DECLARE_MULTICAST_DELEGATE(FOnCharacterPrepareAction);
DECLARE_MULTICAST_DELEGATE(FOnCharacterBonusAction);
DECLARE_MULTICAST_DELEGATE(FOnCharacterPrepareBonusAction);
DECLARE_MULTICAST_DELEGATE(FOnTurnReceive);
DECLARE_MULTICAST_DELEGATE(FOnTurnEnd);
DECLARE_MULTICAST_DELEGATE(FOnInitialized);

UCLASS()
class BG3CPP_API AMoveCharacterBase : public ACharacter, public ISelectableObject
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMoveCharacterBase();
	
	virtual void Initialize();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	class UNavigationPath* ThinkPath(const FVector& dest);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void SetDestination(const FVector& location);

	void ShowInitiative(int32 num);

	virtual void Move();

	void ExecuteAction(class ABG3GameMode* mode,class UCharacterActionData* action, const FVector& targetLocation);

	UFUNCTION()
	virtual void OnMoveCompleted();

	virtual void Selected() override;
	virtual void Deselected() override;

	class UCharacterStatus* GetStatus() const;

	int GetCurrentTurnActionCount() const;
	int GetCurrentBonusActionCount() const;

	int GetMaxTurnActionCount() const;
	int GetMaxBonusActionCount() const;

	bool GetIsTurn() const;

	bool GetIsPrepare() const;

	void TurnReceive();
	void TurnEnd();

	void SetOutline(bool condition);
	
	FOnCharacterAction OnCharacterAction;
	
	FOnCharacterBonusAction OnCharacterBonusAction;
	
	FOnCharacterPrepareAction OnCharacterPrepareAction;

	FOnCharacterPrepareBonusAction OnCharacterPrepareBonusAction;

	FOnTurnReceive OnCharacterTurnReceive;

	FOnTurnEnd OnCharacterTurnEnd;

	FOnInitialized OnInitialized;
	
	const bool* MovablePtr;

	const int32* CurHPPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FObjectStatus Status;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TableName;
	
protected:
	UPROPERTY()
	class UDataTable* DataTable;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterialInterface* SelectedMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UInitiativeUI> InitiativeClass;

	UPROPERTY()
	class UInitiativeUI* InitiativeUI;
	
	UPROPERTY()
	class UMaterialInstanceDynamic* SelectedMatDynamic;

	UPROPERTY()
	FVector Destination;

	float LastMoveDistance;
	float CurrentMOV;
	
	bool bIsMovable = true;
	bool bIsTurn = false;
	bool bIsPrepareAction = false;
	int32 CurHP;

	UPROPERTY()
	class AMovableCharacterController* charController;
	
	UPROPERTY(EditAnywhere)
	class UCharacterStatus* DetailStatus;
	
	TArray<FGameAction> Actions;
	
	int CurTurnActionCount = 1;
	int CurTurnBonusActionCount = 1;

	int MaxTurnActionCount = 1;
	int MaxTurnBonusActionCount = 1;
};
