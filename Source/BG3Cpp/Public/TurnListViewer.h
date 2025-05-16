// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIListViewer.h"
#include "TurnListViewer.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UTurnListViewer : public UUIListViewer
{
	GENERATED_BODY()

public:

	int32 CharacterCount = 0;
	
	virtual void OnDataFetched() override;
	
	virtual void InitializeItem() override;

	virtual void MoveCursor(int32 Gap, bool bIsRefresh = false) override;

	virtual void RefreshOnDataFetched() override;

	void SetSelectedItems();

	int32 GetCursorByTurnListPtr(class UTurnCharacterList* target, TArray<class UTurnCharacterList*> list);

	UPROPERTY()
	TArray<class UTurnPortraitItem*> SelectedItems;

	UPROPERTY()
	TArray<class UTurnPortraitItem*> CastedSpawnItems;

	UPROPERTY()
	class UTurnCharacterList* SelectedTurnList = nullptr;

	UPROPERTY()
	TArray<class UTurnCharacterList*> CachedList;

	FDelegateHandle TurnHandle;
};
