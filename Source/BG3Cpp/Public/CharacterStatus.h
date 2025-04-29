// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BG3Enums.h"
#include "BG3Struct.h"
#include "CharacterStatus.generated.h"


UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BG3CPP_API UCharacterStatus : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacterStatus();

	int32 GetHp() const;

	FString GetName() const;

	TArray<FGameAction*> GetActions() const;

	void Initialize(FObjectStatus status);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	UPROPERTY(EditAnywhere)
	FString FileName;
	
	FObjectStatus Status;

	TArray<FGameAction*> Actions;
	
	int32 HP;
	int32 CharLevel;

	int32 CreateHPwithDice();

	void CreateActions();
	
	void Save();
	bool Load();
};
