// Fill out your copyright notice in the Description page of Project Settings.


#include "NonPlayableCharacterBase.h"

void ANonPlayableCharacterBase::Selected()
{
	Super::Selected();

	//GetMesh()->SetOverlayMaterial(SelectedMatDynamic);
}

void ANonPlayableCharacterBase::Deselected()
{
	Super::Deselected();

	//GetMesh()->SetOverlayMaterial(nullptr);
}

void ANonPlayableCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	Initialize();
}
