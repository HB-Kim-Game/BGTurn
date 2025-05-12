// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpCursor.h"

#include "BGUtil.h"
#include "CharacterStatus.h"
#include "MouseControlledPlayer.h"
#include "ParabolaSpline.h"
#include "PlayableCharacterBase.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"

void UJumpCursor::InitializeCursor(float maxDistance, UCharacterActionData* data)
{
	MaxDistance = maxDistance;
	Data = data;
}

void UJumpCursor::ShowJumpDescription(int32 damage, const FString& description, bool isHigh)
{
	if (isHigh)
	{
		CursorImage->SetBrushFromTexture(HighIcon);
		CursorImage->SetDesiredSizeOverride(FVector2D(41.f, 40.f));
	}
	else
	{
		CursorImage->SetBrushFromTexture(DefaultIcon);
		CursorImage->SetDesiredSizeOverride(FVector2D(41.f, 40.f));
	}

	if (damage > 0)
	{
		FString text = "<ActionPercent>-" + FString::FromInt(damage) + " HP</>";
		Damage->SetText(FText::FromString(text));
		Damage->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		Damage->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (!description.IsEmpty())
	{
		FString text = "<CursorDesc>" + FString::Printf(TEXT("%s"), *description)+ "</>";
		Description->SetText(FText::FromString(text));
		Description->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		Description->SetVisibility(ESlateVisibility::Collapsed);
	}
}

float UJumpCursor::GetMaxDistance() const
{
	return MaxDistance;
}

class UCharacterActionData* UJumpCursor::GetActionData() const
{
	return Data;
}

void UJumpCursor::NativeConstruct()
{
	Super::NativeConstruct();

	player = Cast<AMouseControlledPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

void UJumpCursor::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (nullptr == player && !player->IsValidLowLevel()) return;

	auto* pc = GetWorld()->GetFirstPlayerController();
	if (pc)
	{
		FHitResult hit;
		
		if (pc->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_WorldDynamic), false, hit))
		{
			FVector dir = hit.Location - player->GetPlayableCharacter()->GetActorLocation();
			dir.Z = 0;
			FRotator rotation = dir.Rotation();
			player->GetPlayableCharacter()->SetActorRotation(rotation);
			
			float fallingDistance = player->GetPlayableCharacter()->GetActorLocation().Z - hit.ImpactPoint.Z;
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
			
			int32 damageResult = 0;
			// 4m 아래 이상
			if (fallingDistance >= 400.f)
			{
				damageResult = UBGUtil::CalculateFallingDamage(player->GetPlayableCharacter()->GetStatus()->GetHp(), fallingDistance);
			}

			if (bIsBlocked)
			{
				ShowJumpDescription(0, TEXT("경로가 가로막혔습니다!"));
				CursorImage->SetBrushFromTexture(HighIcon);
				CursorImage->SetDesiredSizeOverride(FVector2D(41.f, 40.f));
				Description->SetVisibility(ESlateVisibility::HitTestInvisible);
				return;
			}
			
			if (FVector::DistXY(player->GetPlayableCharacter()->GetActorLocation(), hit.ImpactPoint) / 100.f > MaxDistance)
			{
				ShowJumpDescription(damageResult, TEXT("거리가 너무 멉니다!"));
				CursorImage->SetBrushFromTexture(HighIcon);
				CursorImage->SetDesiredSizeOverride(FVector2D(41.f, 40.f));
				Description->SetVisibility(ESlateVisibility::HitTestInvisible);
				return;
			}
			// 뛸 수 있는 거리보다 높음
			if (fallingDistance / 100.f <= -MaxDistance)
			{
				ShowJumpDescription(0, TEXT("너무 높습니다!"), true);
				return;
			}
		
			ShowJumpDescription(damageResult);	
		}
	}
}
