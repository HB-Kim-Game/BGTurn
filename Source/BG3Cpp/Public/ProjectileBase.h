// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

UCLASS()
class BG3CPP_API AProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void Initialize(class UCharacterActionData* data, class AMoveCharacterBase* instigator, class AMoveCharacterBase* target);
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UNiagaraSystem* HitSystem;

	FDelegateHandle ExecuteActionHandle;

	virtual void TickAction(float DeltaTime) {}

	bool bIsInitialized = false;
	
	UPROPERTY()
	class UCharacterActionData* ActionData;
	UPROPERTY()
	class AMoveCharacterBase* AttackInstigator;
	UPROPERTY()
	class AMoveCharacterBase* Target;

	void SpawnHitEffect(FVector location);
};
