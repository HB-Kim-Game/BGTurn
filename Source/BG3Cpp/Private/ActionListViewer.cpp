// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionListViewer.h"

#include "BGUtil.h"
#include "MouseControlledPlayer.h"
#include "PlayableCharacterBase.h"
#include "BG3Enums.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"

void UActionListViewer::OnDataFetched()
{
	Super::OnDataFetched();

	for (auto* item : SpawnItems)
	{
		item->FetchData(nullptr);
	}
	
	MoveCursor(0);
}

void UActionListViewer::InitializeItem()
{
	Super::InitializeItem();

	auto* grid = Cast<UGridPanel>(Panel);

	if (grid)
	{
		for (int i = 0; i < SpawnItems.Num(); i++)
		{
			UGridSlot* slot = grid->AddChildToGrid(SpawnItems[i]);
			
			slot->SetRow(i / grid->ColumnFill.Num());
			slot->SetColumn(i % grid->ColumnFill.Num());
		}
	}

	if (SpawnItems.Num() > FetchedDatas.Num())
	{
		for (int i = 0; i< FetchedDatas.Num(); i++)
		{
			SpawnItems[i]->FetchData(FetchedDatas[UBGUtil::ClampCursor(GetCursor() - CursorOffset + i, FetchedDatas.Num())]);
		}
	}
	else
	{
		for (int i = 0; i< SpawnItems.Num(); i++)
		{
			SpawnItems[i]->FetchData(FetchedDatas[UBGUtil::ClampCursor(GetCursor() - CursorOffset + i, FetchedDatas.Num())]);
		}
	}


}

void UActionListViewer::NativeConstruct()
{
	Super::NativeConstruct();

	Player = Cast<AMouseControlledPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

void UActionListViewer::MoveCursor(int32 Gap, bool bIsRefresh)
{
	auto* p = Player->GetPlayableCharacter();
	
	if (SpawnItems.Num() > FetchedDatas.Num())
	{
		for (int i = 0; i< FetchedDatas.Num(); i++)
		{
			auto* data = FetchedDatas[UBGUtil::ClampCursor(GetCursor() - CursorOffset + i, FetchedDatas.Num())];
			SpawnItems[i]->FetchData(data);

			if (auto* cast = Cast<UCharacterActionData>(data))
			{
				if (!p->GetIsTurn())
				{
					SpawnItems[i]->Deselected();
					continue;
				}
				
				switch (cast->ActionCase)
				{
					case EActionCase::DefaultAction:
						if (p->GetCurrentTurnActionCount() <= 0) SpawnItems[i]->Deselected(); 
						else
						{
							SpawnItems[i]->Selected();
							switch (cast->SkillCase)
							{
							case ESkillCase::SpellOne:
								if (p->GetCurrentSpellCount(ESkillCase::SpellOne) <= 0 || !p->GetIsTurn()) SpawnItems[i]->Deselected();
								else SpawnItems[i]->Selected();
								break;
							case ESkillCase::SpellTwo:
								if (p->GetCurrentSpellCount(ESkillCase::SpellTwo) <= 0 || !p->GetIsTurn()) SpawnItems[i]->Deselected();
								else SpawnItems[i]->Selected();
								break;
							}	
						}
						break;
					case EActionCase::BonusAction:
						if (p->GetCurrentBonusActionCount() <= 0) SpawnItems[i]->Deselected();
						else
						{
							SpawnItems[i]->Selected();
							
							if (cast->ActionID == TEXT("Jump"))
							{
								if (p->GetCurrentMOV() < 3.f) SpawnItems[i]->Deselected();
							}
							
							switch (cast->SkillCase)
							{
							case ESkillCase::SpellOne:
								if (p->GetCurrentSpellCount(ESkillCase::SpellOne) <= 0 || !p->GetIsTurn()) SpawnItems[i]->Deselected();
								else SpawnItems[i]->Selected();
								break;
							case ESkillCase::SpellTwo:
								if (p->GetCurrentSpellCount(ESkillCase::SpellTwo) <= 0 || !p->GetIsTurn()) SpawnItems[i]->Deselected();
								else SpawnItems[i]->Selected();
								break;
							}	
						}
						break;
				}
			}
		}
	}
	else
	{
		for (int i = 0; i< SpawnItems.Num(); i++)
		{
			auto* data = FetchedDatas[UBGUtil::ClampCursor(GetCursor() - CursorOffset + i, FetchedDatas.Num())];
			SpawnItems[i]->FetchData(data);

			if (auto* cast = Cast<UCharacterActionData>(data))
			{
				if (!p->GetIsTurn())
				{
					SpawnItems[i]->Deselected();
					continue;
				}
				
				switch (cast->ActionCase)
				{
				case EActionCase::DefaultAction:
					if (p->GetCurrentTurnActionCount() <= 0) SpawnItems[i]->Deselected(); 
					else
					{
						SpawnItems[i]->Selected();
						switch (cast->SkillCase)
						{
						case ESkillCase::SpellOne:
							if (p->GetCurrentSpellCount(ESkillCase::SpellOne) <= 0 || !p->GetIsTurn()) SpawnItems[i]->Deselected();
							else SpawnItems[i]->Selected();
							break;
						case ESkillCase::SpellTwo:
							if (p->GetCurrentSpellCount(ESkillCase::SpellTwo) <= 0 || !p->GetIsTurn()) SpawnItems[i]->Deselected();
							else SpawnItems[i]->Selected();
							break;
						}	
					}
					break;
				case EActionCase::BonusAction:
					if (p->GetCurrentBonusActionCount() <= 0) SpawnItems[i]->Deselected();
					else
					{
						SpawnItems[i]->Selected();
						switch (cast->SkillCase)
						{
						case ESkillCase::SpellOne:
							if (p->GetCurrentSpellCount(ESkillCase::SpellOne) <= 0 || !p->GetIsTurn()) SpawnItems[i]->Deselected();
							else SpawnItems[i]->Selected();
							break;
						case ESkillCase::SpellTwo:
							if (p->GetCurrentSpellCount(ESkillCase::SpellTwo) <= 0 || !p->GetIsTurn()) SpawnItems[i]->Deselected();
							else SpawnItems[i]->Selected();
							break;
						}	
					}
					break;
				}
			}
		}
	}
}
