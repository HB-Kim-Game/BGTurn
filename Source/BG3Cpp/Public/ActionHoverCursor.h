// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BG3Struct.h"
#include "CursorBase.h"
#include "ActionHoverCursor.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UActionHoverCursor : public UCursorBase
{
	GENERATED_BODY()

	public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DisplayName;
	UPROPERTY(meta = (BindWidget))
	class URichTextBlock* DescriptionText;

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* ActionTypeDetailBox;

	UPROPERTY(meta = (BindWidget))
	class UImage* IconImage;
	
	void ShowActionDetail(class UCharacterActionData* action);
};
