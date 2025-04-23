// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableCharacterBase.h"

void APlayableCharacterBase::Selected()
{
	Super::Selected();

	GetMesh()->SetOverlayMaterial(SelectedMatDynamic);
}

void APlayableCharacterBase::Deselected()
{
	Super::Deselected();

	GetMesh()->SetOverlayMaterial(nullptr);
}
