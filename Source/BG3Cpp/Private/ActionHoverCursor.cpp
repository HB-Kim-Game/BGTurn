// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionHoverCursor.h"

#include "ActionTypeDescriptionUI.h"
#include "CharacterActionData.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"

void UActionHoverCursor::ShowActionDetail(UCharacterActionData* action)
{
	DisplayName->SetText(FText::FromString(action->DisplayName));
	FString lineBreakText = "<br>";
	FString lineBreak = "\r\n";
	FString result = action->Description.Replace(*lineBreakText, *lineBreak);
	DescriptionText->SetText(FText::FromString(result));

	if (action->Texture)
	{
		FSlateBrush temp;
		temp.SetResourceObject(action->Texture);
		temp.SetImageSize(FVector2D(100.f, 100.f));
		IconImage->SetBrush(temp);	
	}

	detailCount = 0;

	if (action->MaxDistance > 0.1f)
	{
		if (ActionTypeDetailItems.Num() - 1 < detailCount)
		{
			UActionTypeDescriptionUI* item = Cast<UActionTypeDescriptionUI>(CreateWidget(this, ActionTypeItemClass));
			if (item)
			{
				ActionTypeDetailItems.Add(item);
				{
					UHorizontalBoxSlot* slot = ActionTypeDetailBox->AddChildToHorizontalBox(item);
					slot->SetPadding(FVector2D(5.5f, 5.5f));
		
					slot->SetVerticalAlignment(VAlign_Center);
					slot->SetHorizontalAlignment(HAlign_Left);
					item->SetVisibility(ESlateVisibility::Collapsed);
				}
			}
		}
		
		if (auto* findItem = ActionTypeIconList.Find(TEXT("Distance")))
		{
			FString text = FString::FromInt(action->MaxDistance) + TEXT("m");
			ActionTypeDetailItems[detailCount]->SetActionType(findItem->Texture, text);	
			ActionTypeDetailItems[detailCount]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			detailCount++;
		}
	}

	switch (action->ActionCase)
	{
	case EActionCase::DefaultAction:
		if (auto* findItem = ActionTypeIconList.Find(TEXT("DefaultAction")))
		{
			ActionCaseItem->SetActionType(findItem->Texture, findItem->Description);
		}
		break;
	case EActionCase::BonusAction:
		if (auto* findItem = ActionTypeIconList.Find(TEXT("BonusAction")))
		{
			ActionCaseItem->SetActionType(findItem->Texture, findItem->Description);
		}
		break;
	}
}

void UActionHoverCursor::NativeConstruct()
{
	Super::NativeConstruct();

	if (bIsInitialize) return;
	
	ActionCaseItem = Cast<UActionTypeDescriptionUI>(CreateWidget(this, ActionTypeItemClass));
	UHorizontalBoxSlot* caseItemSlot = ActionCaseBox->AddChildToHorizontalBox(ActionCaseItem);
	caseItemSlot->SetPadding(FVector2D(5.5f, 5.5f));
	caseItemSlot->SetVerticalAlignment(VAlign_Center);
	caseItemSlot->SetHorizontalAlignment(HAlign_Left);

	for (int32 i = 0; i < 2; i++)
	{
		UActionTypeDescriptionUI* item = Cast<UActionTypeDescriptionUI>(CreateWidget(this, ActionTypeItemClass));
		if (item)
		{
			ActionTypeDetailItems.Add(item);
		}
	}
	
	for (int i = 0; i< ActionTypeDetailItems.Num(); i++)
	{
		UHorizontalBoxSlot* slot = ActionTypeDetailBox->AddChildToHorizontalBox(ActionTypeDetailItems[i]);
		slot->SetPadding(FVector2D(5.5f, 5.5f));
		slot->SetVerticalAlignment(VAlign_Center);
		slot->SetHorizontalAlignment(HAlign_Left);

		ActionTypeDetailItems[i]->SetVisibility(ESlateVisibility::Collapsed);
	}

	bIsInitialize = true;
}
