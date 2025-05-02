// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackRange.h"

#include "Components/DecalComponent.h"

AAttackRange::AAttackRange()
{
	ConstructorHelpers::FObjectFinder<UMaterialInstance> material(TEXT("/Game/BG3/Materials/M_RangeDecal_Inst.M_RangeDecal_Inst"));

	if (material.Succeeded())
	{
		DecalMaterial = UMaterialInstanceDynamic::Create(material.Object,this);
		GetDecal()->SetDecalMaterial(DecalMaterial);
	}

	GetDecal()->DecalSize = FVector(500.f, 100.f, 100.f);
}

void AAttackRange::SetDecalRange(float Distance)
{
	GetDecal()->SetWorldScale3D(FVector(Distance));
	DecalMaterial->SetScalarParameterValue(TEXT("Scale"), Distance);
}
