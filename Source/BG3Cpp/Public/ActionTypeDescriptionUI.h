// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ActionTypeDescriptionUI.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UActionTypeDescriptionUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* IconImage;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* DescriptionText;

	void SetActionType(UTexture2D* texture, FString text);
};
