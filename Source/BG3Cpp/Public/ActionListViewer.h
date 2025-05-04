// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIListViewer.h"
#include "CharacterActionData.h"
#include "ActionListViewer.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UActionListViewer : public UUIListViewer
{
	GENERATED_BODY()

protected:

	virtual void OnDataFetched() override;
	
	virtual void InitializeItem() override;

	virtual void NativeConstruct() override;
	
public:
	virtual void MoveCursor(int32 Gap) override;

private:
	UPROPERTY()
	class AMouseControlledPlayer* Player;
};
