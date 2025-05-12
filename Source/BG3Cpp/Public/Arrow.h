// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Arrow.generated.h"

UCLASS()
class BG3CPP_API AArrow : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AArrow();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* StaticMeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UNiagaraSystem* HitSystem;
	
	FDelegateHandle ExecuteActionHandle;

	void Initialize(class UCharacterActionData* data, class AMoveCharacterBase* instigator);

private:
	bool bIsInitialized = false;

	UPROPERTY()
	class UCharacterActionData* ActionData;
	UPROPERTY()
	class AMoveCharacterBase* ArrowInstigator;

	void SpawnHitEffect(FVector location);
};
