// Fill out your copyright notice in the Description page of Project Settings.


#include "NonPlayableCharacterBase.h"

#include "ActionManager.h"
#include "BattleTurnManager.h"
#include "BG3GameMode.h"
#include "CharacterStatus.h"
#include "CustomTimer.h"
#include "MovableCharacterController.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "PlayableCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMeshSocket.h"

ANonPlayableCharacterBase::ANonPlayableCharacterBase()
{
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	FString socketName = TEXT("DefaultWeaponSocket");
	
	WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName(*socketName));
}

void ANonPlayableCharacterBase::Selected()
{
	Super::Selected();

	//GetMesh()->SetOverlayMaterial(SelectedMatDynamic);
}

void ANonPlayableCharacterBase::Deselected()
{
	Super::Deselected();

	//GetMesh()->SetOverlayMaterial(nullptr);
}

void ANonPlayableCharacterBase::ThinkAction()
{
	EnemyCharacters.Empty();
	
	if (Mode)
	{
		UNavigationPath* path = nullptr;
		
		for (auto c  :Mode->BattleManager->Characters)
		{
			if (Cast<APlayableCharacterBase>(c.Character))
			{
				EnemyCharacters.Add(c.Character);
			}
		}

		FVector bestLocation = FVector::ZeroVector;
		FVector TargetLocation = FVector::ZeroVector;
		float bestDistance = FLT_MAX;
		UCharacterActionData* bestActionData = nullptr;

		for (auto* e : EnemyCharacters)
		{
			for (auto* action : Actions)
			{
				if (action->SkillCase == ESkillCase::Buff) continue;
				
				FVector center = e->GetActorLocation();
				float radius = action->MaxDistance * 100.f + GetCapsuleComponent()->GetScaledCapsuleRadius();
				TArray<FVector> points;
				for (int i = 0; i < 12; i++)
				{
					float Angle = i * (2 * PI / 12);
					FVector Offset = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0) * radius;
					FVector point = center + Offset;
					points.Add(point);
				}
				UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

				for (const FVector& point : points)
				{
					FNavLocation projected;
					if (NavSys->ProjectPointToNavigation(point, projected))
					{
						UNavigationPath* p = UNavigationSystemV1::FindPathToLocationSynchronously(GetWorld(), GetActorLocation(), projected.Location);

						if (p && p->IsValid() && !p->IsPartial())
						{
							//DrawDebugSphere(GetWorld(), projected, GetCapsuleComponent()->GetScaledCapsuleRadius(), 20, FColor::Yellow, false, 3.f, 0, 1);
							FHitResult hit;
							if (GetWorld()->SweepSingleByChannel(hit, projected, projected, FQuat::Identity, ECC_EngineTraceChannel1, FCollisionShape::MakeSphere(GetCapsuleComponent()->GetScaledCapsuleRadius())))
							{
								if (hit.GetActor() != e) continue;
							}
							
							if (p->GetPathLength() < bestDistance)
							{
								if (action->TargetCase == ETargetCase::Single)
								{
									action->Target.Empty();
									action->Target.Add(e);
									TargetLocation = e->GetActorLocation();
								}
								else
								{
									action->Target.Add(e);
								}
								bestDistance = p->GetPathLength();
								bestLocation = projected.Location;
								bestActionData = action;
							}
						}
					}
				}

				if (bestLocation.IsNearlyZero()) continue;
				UNavigationPath* tempPath = ThinkPath(bestLocation);
			
				if (path == nullptr)
				{
					path = tempPath;
				}
				else if (tempPath && tempPath->GetPathLength() < path->GetPathLength())
				{
					path = tempPath;
				}	
			}
		}

		if (path && path->PathPoints.Num() >= 2)
		{
			if (path->GetPathLength() < GetCapsuleComponent()->GetScaledCapsuleRadius() + 35.f)
			{
				if (GetCurrentTurnActionCount() > 0)
				{
					FVector dir = TargetLocation - GetActorLocation();
					dir.Z = 0;
					FRotator rotation = dir.Rotation();
					SetActorRotation(rotation);
					Mode->ActionManager->ExecuteAction(bestActionData, this);

					UCustomTimer::SetTimer(this->GetWorld(), 1.0f, [](float alpha)
					{
					},
					[this]()
					{
						ThinkAction();
					});
					return;
				}
				TurnEnd();
				return;
			}
			
			float distanceResult = 0.0f;
			
			for (int i = 1; i < path->PathPoints.Num(); ++i)
			{
				const FVector& prevPoint = path->PathPoints[i - 1];
				const FVector& currentPoint = path->PathPoints[i];
				float segLength = FVector::Dist(prevPoint, currentPoint);

				if ((distanceResult + segLength) / 100.f > CurrentMOV)
				{
					if (GetCurrentTurnActionCount() > 0)
					{
						auto* SprintAction = Actions.FindByPredicate([](UCharacterActionData* a)
						{
							return a->ActionID == TEXT("Sprint");
						});
						if (SprintAction)
						{
							Mode->ActionManager->ExecuteAction(*SprintAction, this);
							UCustomTimer::SetTimer(this->GetWorld(), 1.0f, [](float alpha)
							{
							},
							[this]()
							{
								ThinkAction();
							});
							return;
						}
					}
					
					if (auto* cast = Cast<AMovableCharacterController>(GetController()))
					{
						MoveDelegateHandle = cast->OnAIMoveCompleted.Add(FSimpleDelegate::CreateLambda([this, cast]()
						{
							this->TurnEnd();
							bool bIsSuccess = cast->OnAIMoveCompleted.Remove(MoveDelegateHandle);
						}));
					}
					
					float remain = CurrentMOV - distanceResult;
					float alpha = remain / segLength;
					SetDestination(FMath::Lerp(prevPoint, currentPoint, alpha));
					Move();
					return;
				}
				
				distanceResult += segLength;
			}

			if (auto* cast = Cast<AMovableCharacterController>(GetController()))
			{
				MoveDelegateHandle = cast->OnAIMoveCompleted.Add(FSimpleDelegate::CreateLambda([this, cast]()
				{
					UCustomTimer::SetTimer(this->GetWorld(), 1.0f, [](float alpha)
					{
					},
					[this]()
					{
						if (GetCurrentTurnActionCount() <= 0)
						{
							TurnEnd();
						}
					});
					bool bIsSuccess = cast->OnAIMoveCompleted.Remove(MoveDelegateHandle);
				}));
			}

			if (GetCurrentTurnActionCount() <= 0)
			{
				SetDestination(bestLocation);
				Move();
				return;
			}
	
			ExecuteAction(Mode, bestActionData, TargetLocation);
		}
		else
		{
			TurnEnd();
		}
	}
	else
	{
		TurnEnd();
	}
}

void ANonPlayableCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	Initialize();

	Mode = Cast<ABG3GameMode>(GetWorld()->GetAuthGameMode());
	
}

void ANonPlayableCharacterBase::Initialize()
{
	Super::Initialize();

	for (auto action : DetailStatus->GetActions())
	{
		UCharacterActionData* data = NewObject<UCharacterActionData>();
		data->ActionID = action->ActionID;
		data->ActionCase = action->ActionCase;
		data->SkillCase = action->SkillCase;
		data->TargetCase = action->TargetCase;
		data->MaxTargetCount = action->MaxTargetCount;
		data->UpcastNum = action->UpcastNum;
		data->CurMaxTargetCount = action->MaxTargetCount;
		
		data->MaxDistance = action->MaxDistance;
		data->MinDistance = action->MinDistance;
		data->AmountTurn = action->AmountTurn;
		
		data->DisplayName = action->DisplayName;
		data->Description = action->Description;
		data->Texture = action->Texture;
		data->TexturePath = action->TexturePath;

		Actions.Add(data);
	}
}

void ANonPlayableCharacterBase::OnMontageEnded(class UAnimMontage* Montage, bool bInterrupted)
{
	Super::OnMontageEnded(Montage, bInterrupted);
}
