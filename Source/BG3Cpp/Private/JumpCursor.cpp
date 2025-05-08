// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpCursor.h"

#include "BGUtil.h"
#include "CharacterStatus.h"
#include "MouseControlledPlayer.h"
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
		Description->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		CursorImage->SetBrushFromTexture(DefaultIcon);
		CursorImage->SetDesiredSizeOverride(FVector2D(41.f, 40.f));
		Description->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (damage > 0)
	{
		Damage->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		Damage->SetVisibility(ESlateVisibility::Collapsed);
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
		float fallingDistance = player->GetPlayableCharacter()->GetActorLocation().Z - hit.ImpactPoint.Z;

		int32 damageResult = 0;
		// 4m 아래 이상
		if (fallingDistance >= 400.f)
		{
			damageResult = UBGUtil::CalculateFallingDamage(player->GetPlayableCharacter()->GetStatus()->GetHp(), fallingDistance);
		}
			
		if (FVector::DistXY(player->GetPlayableCharacter()->GetActorLocation(), hit.ImpactPoint) / 100.f > MaxDistance)
		{
			ShowJumpDescription(damageResult, TEXT("너무 멉니다!"));
			CursorImage->SetBrushFromTexture(HighIcon);
			CursorImage->SetDesiredSizeOverride(FVector2D(41.f, 40.f));
			Description->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		// 뛸 수 있는 거리보다 높음
		if (fallingDistance <= -MaxDistance)
		{
			ShowJumpDescription(0, TEXT("너무 높습니다!"), true);
			return;
		}
		
		ShowJumpDescription(damageResult);	
	}
}
