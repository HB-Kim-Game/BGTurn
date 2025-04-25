// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerUI.h"

#include "Components/TextBlock.h"

void UPlayerUI::ShowSelectedCharHP(int32 curHp, int32 maxHp)
{
	FString str = FString::FromInt(curHp) + " / " + FString::FromInt(maxHp);
	
	SelectedCharHP->SetText(FText::FromString(str));
}
