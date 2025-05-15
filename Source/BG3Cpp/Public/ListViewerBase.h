// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ListViewerBase.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UListViewerBase : public UUserWidget
{
	GENERATED_BODY()

public:
	int32 GetCursor() const;

	void SetCursor(int32 value);

	UObject* GetSelectedItem() const;

	virtual void OnDataFetched() {}

	template<typename T>
	void FetchDatas(const TArray<T*>& datas)
	{
		FetchedDatas.Empty();
		for (T* data : datas)
		{
			if (data)
			{
				auto* cast = Cast<UObject>(data);
				FetchedDatas.Add(cast);
			}
		}

		OnDataFetched();
	}

	virtual void MoveCursor(int32 Gap, bool bIsRefresh = false);
	
	
protected:

	UPROPERTY()
	TArray<UObject*> FetchedDatas;
	
	int32 Cursor = 0;
};
