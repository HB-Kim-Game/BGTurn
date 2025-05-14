// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionCursor.h"
#include "MultiTargetActionCursor.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UMultiTargetActionCursor : public UActionCursor
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UImage* TargetProgress;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMaterialInterface* TargetMaterial;
	
	UPROPERTY()
	UMaterialInstanceDynamic* TargetMatDynamic;

	virtual void ShowActionDescription(class UCharacterActionData* action, int percent, const FString& description = L"") override;

	virtual void ShowCursorInfo() override;
	
	void ShowTargetProgress(int addNum);
	
	int CurTargetNum = 0;
	
protected:
	virtual void NativeConstruct() override;
};
