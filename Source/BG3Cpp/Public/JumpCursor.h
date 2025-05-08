// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CursorBase.h"
#include "JumpCursor.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UJumpCursor : public UCursorBase
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	class URichTextBlock* Damage;

	UPROPERTY(meta = (BindWidget))
	class URichTextBlock* Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* DefaultIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* HighIcon;

	void InitializeCursor(float maxDistance, class UCharacterActionData* data);

	void ShowJumpDescription(int32 damage = 0, const FString& description = TEXT(""), bool isHigh = false);

	float GetMaxDistance() const;

	class UCharacterActionData* GetActionData() const;

protected:
	virtual void NativeConstruct() override;;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	float MaxDistance;
	UPROPERTY()
	class UCharacterActionData* Data;

	UPROPERTY()
	class AMouseControlledPlayer* player;
};
