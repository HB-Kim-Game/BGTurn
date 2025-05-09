// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MoveCharacterBase.h"
#include "GameFramework/Actor.h"
#include "FireBall.generated.h"

UCLASS()
class BG3CPP_API AFireBall : public AActor
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UNiagaraSystem* HitSystem;
	
	FDelegateHandle ExecuteActionHandle;

	void Initialize(class UCharacterActionData* data, class AMoveCharacterBase* instigator);

private:
	bool bIsInitialized = false;

	UPROPERTY()
	class UCharacterActionData* ActionData;
	UPROPERTY()
	class AMoveCharacterBase* FireballInstigator;
};
