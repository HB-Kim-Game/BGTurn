// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BG3Struct.h"
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

	UPROPERTY(meta = (BindWidget))
	class URichTextBlock* BonusDescription;

	UPROPERTY(meta = (BindWidget))
	class UImage* BonusIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* BonusTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* MinusTexture;

	class UCharacterActionData* GetAction() const;
	
	virtual void ShowActionDescription(class UCharacterActionData* action, int percent,const FString& description = TEXT(""));

	virtual void ShowActionBonus(int bonus, const FString& description = TEXT(""));

	virtual void ShowCursorInfo();

protected:
	virtual void NativeConstruct() override;;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY()
	class UCharacterActionData* Action;

	UPROPERTY()
	class AMouseControlledPlayer* player;
};
