// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MoveCharacterBase.h"
#include "ProjectileBase.h"
#include "GameFramework/Actor.h"
#include "FireBall.generated.h"

UCLASS()
class BG3CPP_API AFireBall : public AProjectileBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFireBall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UNiagaraComponent* NiagaraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UNiagaraSystem* NiagaraSystem;

protected:
	virtual void TickAction(float DeltaTime) override;
};
