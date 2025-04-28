// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ViewerItemBase.generated.h"

/**
 * 
 */
UCLASS()
class BG3CPP_API UViewerItemBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void FetchData(UObject* Data) {}
	virtual void FetchStructData(const void* Data) {}
	virtual void Selected() {}
	virtual void Deselected() {}
};
