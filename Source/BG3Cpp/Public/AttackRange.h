// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DecalActor.h"
#include "AttackRange.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API AAttackRange : public ADecalActor
{
	GENERATED_BODY()

public:
	AAttackRange();
	void SetDecalRange(float Distance);
	
	UPROPERTY()
	class UMaterialInstanceDynamic* DecalMaterial;
};
