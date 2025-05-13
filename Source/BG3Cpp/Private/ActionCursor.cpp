// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionCursor.h"

#include "CharacterActionData.h"
#include "MouseControlledPlayer.h"
#include "MoveCharacterBase.h"
#include "ParabolaSpline.h"
#include "PlayableCharacterBase.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/RichTextBlock.h"

UCharacterActionData* UActionCursor::GetAction() const
{
	return Action;
}

void UActionCursor::ShowActionDescription(UCharacterActionData* action, int percent,const FString& description)
{
	IconImage->SetBrushFromTexture(action->Texture);
	Action = action;
	
	if (percent > 90)
	{
		Percent->SetVisibility(ESlateVisibility::Visible);
		FString text = "<ActionPercentHigh>" + FString::FromInt(percent) + "%</>";
		Percent->SetText(FText::FromString(text));
	}
	else
	{
		if (percent <= 0)
		{
			Percent->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			Percent->SetVisibility(ESlateVisibility::Visible);
			FString text = "<ActionPercent>" + FString::FromInt(percent) + "%</>";
			Percent->SetText(FText::FromString(text));		
		}
	}

	if (description.IsEmpty())
	{
		Description->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		FString text = "<CursorDesc>" + FString::Printf(TEXT("%s"), *description)+ "</>";
		Description->SetText(FText::FromString(text));
		Description->SetVisibility(ESlateVisibility::Visible);
	}
	
}

void UActionCursor::ShowActionBonus(int bonus, const FString& description)
{
	if (bonus == 0)
	{
		BonusIcon->GetParent()->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		BonusIcon->GetParent()->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		if (bonus > 0)
		{
			BonusIcon->SetBrushFromTexture(BonusTexture);
			BonusIcon->SetDesiredSizeOverride(FVector2D(16.f, 16.f));
		}
		else
		{
			BonusIcon->SetBrushFromTexture(MinusTexture);
			BonusIcon->SetDesiredSizeOverride(FVector2D(16.f, 16.f));
		}
		
		FString text = "<CursorDesc>" + FString::Printf(TEXT("%s"), *description)+ "</>";
		BonusDescription->SetText(FText::FromString(text));
	}
}

void UActionCursor::NativeConstruct()
{
	Super::NativeConstruct();

	player = Cast<AMouseControlledPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

void UActionCursor::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (nullptr == player && !player->IsValidLowLevel()) return;
	if (nullptr == Action && !Action->IsValidLowLevel()) return;

	auto* pc = GetWorld()->GetFirstPlayerController();
	if (pc)
	{
		FHitResult hit;

		bool bIsBlocked = false;
		TArray<AActor*> actors;
		player->GetPlayableCharacter()->GetAttachedActors(actors);

		for (auto* actor : actors)
		{
			if (auto* cast = Cast<AParabolaSpline>(actor))
			{
				bIsBlocked = cast->GetIsBlocked();
			}
		}

		if (bIsBlocked)
		{
			ShowActionDescription(Action, 0, TEXT("경로가 가로막혔습니다!"));
			Description->SetVisibility(ESlateVisibility::HitTestInvisible);
			ShowActionBonus(0);
			return;
		}
		
		if (pc->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_WorldDynamic), false, hit))
		{
			FVector dir = hit.Location - player->GetPlayableCharacter()->GetActorLocation();
			dir.Z = 0;
			FRotator rotation = dir.Rotation();
			player->GetPlayableCharacter()->SetActorRotation(rotation);
			
			if (auto* character = Cast<AMoveCharacterBase>(hit.GetActor()))
			{
				float percent;

				// 고지대 보정
				int bonus = player->GetPlayableCharacter()->GetActorLocation().Z >= hit.GetActor()->GetActorLocation().Z + 250.f ? 2 :
								player->GetPlayableCharacter()->GetActorLocation().Z <= hit.GetActor()->GetActorLocation().Z - 250.f ? -2 : 0;

				// 원거리를 근거리로 공격할 경우 보정
				bonus += FVector::Distance(player->GetPlayableCharacter()->GetActorLocation(), hit.GetActor()->GetActorLocation()) / 100.f < Action->MinDistance ? -2 : 0;

				FString bonusDescription = bonus > 0 ? TEXT("유리보정") : TEXT("불리보정");
				switch (Action->SkillCase)
				{
					case ESkillCase::DefaultAttack:
						if (auto* playableCharacter = Cast<APlayableCharacterBase>(character))
						{
							if (player->GetPlayableCharacter() == playableCharacter) break;
						}
						percent = static_cast<float>(20 - character->Status.Defensive + bonus + 2) / 20 * 100.f;
						if (player->GetPlayableCharacter()->GetIsNoPath() && Action->MaxDistance < 2.1f &&
							FVector::Distance(player->GetPlayableCharacter()->GetActorLocation(), character->GetActorLocation()) / 100.f < Action->MaxDistance + player->GetPlayableCharacter()->GetCurrentMOV())
						{
							ShowActionDescription(Action, percent, TEXT("이동할 수 없습니다!"));
							ShowActionBonus(bonus, bonusDescription);
							break;
						}
						if (FVector::Distance(player->GetPlayableCharacter()->GetActorLocation(), character->GetActorLocation()) / 100.f > Action->MaxDistance + player->GetPlayableCharacter()->GetCurrentMOV())
						{
							ShowActionDescription(Action, static_cast<int>(percent), TEXT("거리가 너무 멉니다!"));
							ShowActionBonus(bonus, bonusDescription);
							break;
						}
						ShowActionDescription(Action, static_cast<int>(percent));
						ShowActionBonus(bonus, bonusDescription);
						break;
					case ESkillCase::Buff:
						if (Action->MaxDistance > 0.05f)
						{
							if (FVector::Distance(player->GetPlayableCharacter()->GetActorLocation(), character->GetActorLocation()) / 100.f > Action->MaxDistance)
							{
								ShowActionDescription(Action, 0, TEXT("거리가 너무 멉니다!"));
								break;
							}
						}
						ShowActionDescription(Action, 0);
						break;
					case ESkillCase::SpellOne:
						if (auto* playableCharacter = Cast<APlayableCharacterBase>(character))
						{
							if (player->GetPlayableCharacter() == playableCharacter) break;
						}
						percent = static_cast<float>(20 - character->Status.Defensive + bonus) / 20 * 100.f;
						if (FVector::Distance(player->GetPlayableCharacter()->GetActorLocation(), character->GetActorLocation()) / 100.f > Action->MaxDistance + player->GetPlayableCharacter()->GetCurrentMOV())
						{
							ShowActionDescription(Action, static_cast<int>(percent), TEXT("거리가 너무 멉니다!"));
							ShowActionBonus(bonus, bonusDescription);
							break;
						}
						ShowActionDescription(Action, static_cast<int>(percent));
						ShowActionBonus(bonus, bonusDescription);
						break;
					case ESkillCase::SpellTwo:
						if (auto* playableCharacter = Cast<APlayableCharacterBase>(character))
						{
							if (player->GetPlayableCharacter() == playableCharacter) break;
						}
						percent = static_cast<float>(20 - character->Status.Defensive + bonus) / 20 * 100.f;
						if (FVector::Distance(player->GetPlayableCharacter()->GetActorLocation(), character->GetActorLocation()) / 100.f > Action->MaxDistance + player->GetPlayableCharacter()->GetCurrentMOV())
						{
							ShowActionDescription(Action, static_cast<int>(percent), TEXT("거리가 너무 멉니다!"));
							ShowActionBonus(bonus, bonusDescription);
							break;
						}
						ShowActionDescription(Action, static_cast<int>(percent));
						ShowActionBonus(bonus, bonusDescription);
						break;
				}
			}
			else
			{
				ShowActionBonus(0);
				switch (Action->SkillCase)
				{
				case ESkillCase::DefaultAttack:
					if (player->GetPlayableCharacter()->GetIsNoPath() && Action->MaxDistance < 2.1f)
					{
						ShowActionDescription(Action, 0, TEXT("이동할 수 없습니다!"));
						break;
					}
					if (FVector::Distance(player->GetPlayableCharacter()->GetActorLocation(), hit.ImpactPoint) / 100.f > Action->MaxDistance + player->GetPlayableCharacter()->GetCurrentMOV())
					{
						ShowActionDescription(Action, 0,TEXT("거리가 너무 멉니다!"));
						break;
					}
					
					ShowActionDescription(Action, 0);
					break;
				case ESkillCase::Buff:
					if (Action->MaxDistance > 0.05f)
					{
						if (FVector::Distance(player->GetPlayableCharacter()->GetActorLocation(), hit.ImpactPoint) / 100.f > Action->MaxDistance)
						{
							ShowActionDescription(Action, 0, TEXT("거리가 너무 멉니다!"));
							break;
						}
					}
					ShowActionDescription(Action, 0);
					break;
				case ESkillCase::SpellOne:
					if (FVector::Distance(player->GetPlayableCharacter()->GetActorLocation(), hit.ImpactPoint) / 100.f > Action->MaxDistance + player->GetPlayableCharacter()->GetCurrentMOV())
					{
						ShowActionDescription(Action, 0,TEXT("거리가 너무 멉니다!"));
						break;
					}
					ShowActionDescription(Action, 0);
					break;
				case ESkillCase::SpellTwo:
					if (FVector::Distance(player->GetPlayableCharacter()->GetActorLocation(), hit.ImpactPoint) / 100.f > Action->MaxDistance + player->GetPlayableCharacter()->GetCurrentMOV())
					{
						ShowActionDescription(Action, 0,TEXT("거리가 너무 멉니다!"));
						break;
					}
					ShowActionDescription(Action, 0);
					break;
				}	
			}
		}
	}
	
}
