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
DECLARE_MULTICAST_DELEGATE(FOnDead);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnTakeDefaultDamage, float/* Damage */, AMoveCharacterBase*/* DamagedCharacter */, AMoveCharacterBase* /* Instigator */);

UCLASS()
class BG3CPP_API AMoveCharacterBase : public ACharacter, public ISelectableObject
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMoveCharacterBase();
	
	virtual void Initialize();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, class UAnimSequenceBase*> AnimationMap;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	class UNavigationPath* ThinkPath(const FVector& dest, const FVector& extent = FVector(150.f, 150.f, 200.f));

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void PlayAnimation(const FString& actionID);
	
	void SetDestination(const FVector& location);

	void ShowInitiative(int32 num);

	virtual void Move();

	void ExecuteAction(class ABG3GameMode* mode,class UCharacterActionData* action, const FVector& targetLocation);

	virtual void StopAction();

	void AddMOV(float value, bool isSprint = true);

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

	class UDamageUI* GetDamageUI() const;

	float GetMaxMov() const;

	void TurnReceive();
	void TurnEnd();

	void SetOutline(bool condition);
	void SetIsMovable(bool condition);
	
	FOnCharacterAction OnCharacterAction;
	
	FOnCharacterBonusAction OnCharacterBonusAction;
	
	FOnCharacterPrepareAction OnCharacterPrepareAction;

	FOnCharacterPrepareBonusAction OnCharacterPrepareBonusAction;

	FOnTurnReceive OnCharacterTurnReceive;

	FOnTurnEnd OnCharacterTurnEnd;

	FOnInitialized OnInitialized;

	FOnTakeDefaultDamage OnTakeDefaultDamage;

	FOnDead OnDead;
	
	const bool* MovablePtr;

	const int32* CurHPPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FObjectStatus Status;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TableName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimationAsset* IdleAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimationAsset* MoveAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimationAsset* DeadAnimation;
	
protected:
	UPROPERTY()
	class UDataTable* DataTable;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterialInterface* SelectedMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UInitiativeUI> InitiativeClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UDamageUI> DamageUIClass;

	UPROPERTY()
	class UInitiativeUI* InitiativeUI;

	UPROPERTY()
	class UDamageUI* DamageUI;
	
	UPROPERTY()
	class UMaterialInstanceDynamic* SelectedMatDynamic;

	UPROPERTY()
	FVector Destination;

	float LastMoveDistance;
	float CurrentMOV;
	float MaxMOV;
	
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

	UFUNCTION()
	virtual void OnMontageEnded(class UAnimMontage* Montage, bool bInterrupted);

	FDelegateHandle ExecuteActionHandle;
};
