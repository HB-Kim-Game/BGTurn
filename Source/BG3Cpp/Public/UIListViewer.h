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
	virtual void MoveCursor(int32 Gap) override;
	
	virtual void OnDataFetched() override
	{
		if (SpawnItems.Num() > 0)
		{
			// 새로 패치된 리스트 정보만 다시 뿌려줌.
		}

		if (FetchedDatas.Num() > 0)
		{
			InitializeItem(FetchedDatas);
		}
		else if (FetchedStructDatas.Num() > 0)
		{
			InitializeItem(FetchedStructsDatasRef);
		}
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ListViewer)
	class UPanelWidget* Panel;

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

	void InitializeItem(TArray<UObject*> datas);
	
	template<typename T>
	void InitializeItem(const TArray<T>& datas)
	{
		for (int32 i = 0; i < datas.Num(); i++)
		{
			UViewerItemBase* item = Cast<UViewerItemBase>(CreateWidget(GetWorld(), ItemClass));
			if (item)
			{
				Panel->AddChild(item);
				item->FetchStructData(datas[i]);
				//오프셋 적용 해야함.
				SpawnItems.Add(item);
			}
		}
	}
	
};
