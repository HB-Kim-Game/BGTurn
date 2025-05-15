// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiTargetActionCursor.h"
#include "CharacterActionData.h"
#include "MouseControlledPlayer.h"
#include "ParabolaSpline.h"
#include "PlayableCharacterBase.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"

void UMultiTargetActionCursor::ShowActionDescription(UCharacterActionData* action, int percent,
                                                     const FString& description)
{
	if (action->ActionID == TEXT("MagicMissile"))
	{
		percent = 100.f;
	}
	
	Super::ShowActionDescription(action, percent, description);
}

void UMultiTargetActionCursor::ShowCursorInfo()
{
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
							FVector::Distance(player->GetPlayableCharacter()->GetActorLocation(), character->GetActorLocation()) / 100.f < Action->MaxDistance)
						{
							ShowActionDescription(Action, percent, TEXT("이동할 수 없습니다!"));
							ShowActionBonus(bonus, bonusDescription);
							break;
						}
						if (FVector::Distance(player->GetPlayableCharacter()->GetActorLocation(), character->GetActorLocation()) / 100.f > Action->MaxDistance)
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
						if (FVector::Distance(player->GetPlayableCharacter()->GetActorLocation(), character->GetActorLocation()) / 100.f > Action->MaxDistance)
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
						if (FVector::Distance(player->GetPlayableCharacter()->GetActorLocation(), character->GetActorLocation()) / 100.f > Action->MaxDistance)
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
					if (FVector::Distance(player->GetPlayableCharacter()->GetActorLocation(), hit.ImpactPoint) / 100.f > Action->MaxDistance)
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
					if (FVector::Distance(player->GetPlayableCharacter()->GetActorLocation(), hit.ImpactPoint) / 100.f > Action->MaxDistance)
					{
						ShowActionDescription(Action, 0,TEXT("거리가 너무 멉니다!"));
						break;
					}
					ShowActionDescription(Action, 0);
					break;
				case ESkillCase::SpellTwo:
					if (FVector::Distance(player->GetPlayableCharacter()->GetActorLocation(), hit.ImpactPoint) / 100.f > Action->MaxDistance)
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

void UMultiTargetActionCursor::ShowTargetProgress(int addNum)
{
	if (!Action) return;

	if (CurTargetNum + addNum > Action->MaxTargetCount)
	{
		FString text = "<CursorDesc>" + FString::Printf(TEXT("초과했습니다!"))+ "</>";
		Description->SetText(FText::FromString(text));
		Description->SetVisibility(ESlateVisibility::Visible);
		return;
	}
	
	CurTargetNum += addNum;
	UE_LOG(LogTemp, Warning, TEXT("%f"), static_cast<float>(CurTargetNum) / Action->CurMaxTargetCount);
	TargetMatDynamic->SetScalarParameterValue("Percent", static_cast<float>(CurTargetNum) / Action->CurMaxTargetCount);
}

void UMultiTargetActionCursor::NativeConstruct()
{
	Super::NativeConstruct();

	TargetMatDynamic = UMaterialInstanceDynamic::Create(TargetMaterial, this);
	TargetProgress->SetBrushFromMaterial(TargetMatDynamic);
}
