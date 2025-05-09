// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CursorBase.h"
#include "ActionCursor.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UActionCursor : public UCursorBase
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UImage* IconImage;

	UPROPERTY(meta = (BindWidget))
	class URichTextBlock* Percent;

	UPROPERTY(meta = (BindWidget))
	class URichTextBlock* Description;

	class UCharacterActionData* GetAction() const;
	
	void ShowActionDescription(class UCharacterActionData* action, int percent,const FString& description = TEXT(""));

protected:
	virtual void NativeConstruct() override;;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UPROPERTY()
	class UCharacterActionData* Action;

	UPROPERTY()
	class AMouseControlledPlayer* player;
};
