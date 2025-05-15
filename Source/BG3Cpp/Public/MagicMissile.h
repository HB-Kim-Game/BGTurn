// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "MagicMissile.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API AMagicMissile : public AProjectileBase
{
	GENERATED_BODY()

public:
	AMagicMissile();

protected:
	virtual void BeginPlay() override;

	virtual void TickAction(float DeltaTime) override;

	FVector FindBestDirection();
	
	UPROPERTY()
	class UNiagaraComponent* NiagaraComp;

	UPROPERTY()
	class USphereComponent* SphereComp;
	
	UPROPERTY()
	class UNiagaraSystem* NiagaraSystem;

	float Speed = 2000.f;

	float TraceDistance = 100.f;
	float TurnSpeed = 1000.f;
};
