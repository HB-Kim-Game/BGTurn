// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ListViewerBase.h"
#include "ViewerItemBase.h"
#include "Components/PanelWidget.h"
#include "UIListViewer.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UUIListViewer : public UListViewerBase
{
	GENERATED_BODY()

public:
	virtual void MoveCursor(int32 Gap, bool bIsRefresh = false) override;
	
	virtual void OnDataFetched() override
	{
		if (SpawnItems.Num() > 0)
		{
			// 새로 패치된 리스트 정보만 다시 뿌려줌.
			return;
		}

		InitializeItem();
	}

protected:
	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* Panel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ListViewer)
	int ItemPoolCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ListViewer)
	int CursorOffset = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ListViewer)
	TSubclassOf<UViewerItemBase> ItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ListViewer)
	float VerticalOffset = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ListViewer)
	float HorizontalOffset = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ListViewer)
	float VercialInterval = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ListViewer)
	float HorizontalInterval = 0.0f;

	TArray<UViewerItemBase*> SpawnItems;

	virtual void InitializeItem();
};
