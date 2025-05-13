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
	class UHorizontalBox* ActionCaseBox;

	UPROPERTY(meta = (BindWidget))
	class UImage* IconImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, FUIData> ActionTypeIconList;
	
	void ShowActionDetail(class UCharacterActionData* action);

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UActionTypeDescriptionUI> ActionTypeItemClass;

	UPROPERTY()
	TArray<UActionTypeDescriptionUI*> ActionTypeDetailItems;
	UPROPERTY()
	UActionTypeDescriptionUI* ActionCaseItem;

	int32 detailCount = 0;

	bool bIsInitialize = false;

protected:
	virtual void NativeConstruct() override;
};
