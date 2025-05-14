// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBase.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsInitialized) return;
	
	TickAction(DeltaTime);
}

void AProjectileBase::Initialize(class UCharacterActionData* data, class AMoveCharacterBase* instigator,
	class AMoveCharacterBase* target)
{
	ActionData = data;
	AttackInstigator = instigator;
	Target = target;
	
	bIsInitialized = true;	
}

void AProjectileBase::SpawnHitEffect(FVector location)
{
	if (HitSystem != nullptr)
	{
		UNiagaraComponent* hitEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitSystem, location);
		hitEffect->SetActive(true);
		Destroy();
	}
}

