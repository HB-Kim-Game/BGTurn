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
			if (data) FetchedDatas.Add(data);
		}

		OnDataFetched();
	}

	template<typename T>
	void FetchStructDatas(const TArray<T>& datas)
	{
		FetchedStructDatas.Empty();
		StructSize = sizeof(T);

		if (datas.Num() > 0)
		{
			FetchedStructDatas.AddUninitialized(StructSize * datas.Num());
			FMemory::Memcpy(FetchedStructDatas.GetData(), datas.GetData(), StructSize * datas.Num());
		}

		for (const T& data : datas)
		{
			FetchedStructsDatasRef.Add(static_cast<void*>(&data));
		}
		
		OnDataFetched();
	}

	virtual void MoveCursor(int32 Gap);
	
	
protected:

	UPROPERTY()
	TArray<UObject*> FetchedDatas;
	
	TArray<uint8> FetchedStructDatas;
	int32 StructSize = 0;

	// 레퍼런스형태 저장용 (복사 저장은 쓰기가 까다로움... 일단 보류)
	TArray<void*> FetchedStructsDatasRef;

	template<typename T>
	void GetFetchedStructs(TArray<T>& Out) const
	{
		Out.Empty();
		// 구조체 타입이 맞지 않음.
		if (StructSize != sizeof(T)) return;

		int32 count = FetchedStructDatas.Num() / StructSize;
		if (count > 0)
		{
			Out.AddUninitialized(count);
			FMemory::Memcpy(Out.GetData(), FetchedStructDatas.GetData(), FetchedStructDatas.Num());
		}
	}

private:
	int32 Cursor = 0;
};
