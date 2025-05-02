// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionCountIcon.h"

#include "Components/Image.h"

void UActionCountIcon::SetIconImage(UTexture2D* texture)
{
	IconImage->SetBrushFromTexture(texture);
}

void UActionCountIcon::PlayNormalAnimation()
{
	StopAllAnimations();
	PlayAnimation(NormalIcon, 0, 0);
}

void UActionCountIcon::PlayCostAnimation()
{
	StopAllAnimations();
	PlayAnimation(CostIcon, 0, 0);
}

void UActionCountIcon::PlayUsedAnimation()
{
	StopAllAnimations();
	PlayAnimation(UsedIcon, 0, 0);
}
