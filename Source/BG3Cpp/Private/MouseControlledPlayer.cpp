// Fill out your copyright notice in the Description page of Project Settings.


#include "MouseControlledPlayer.h"

#include "ActionCursor.h"
#include "ActionListViewer.h"
#include "ActionManager.h"
#include "BG3Enums.h"
#include "BG3GameMode.h"
#include "CharacterActionData.h"
#include "CharacterStatus.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "JumpCursor.h"
#include "MouseManager.h"
#include "MovableCharacterController.h"
#include "MoveCursor.h"
#include "MultiTargetActionCursor.h"
#include "NonPlayableCharacterBase.h"
#include "ParabolaSpline.h"
#include "PlayableCharacterBase.h"
#include "PlayerUI.h"
#include "SelectableObject.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMouseControlledPlayer::AMouseControlledPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitializeInput();

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->SetRelativeRotation(FRotator(-42.f, 0, 0));
	SpringArmComp->TargetArmLength = 1600.f;
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
}



// Called when the game starts or when spawned
void AMouseControlledPlayer::BeginPlay()
{
	Super::BeginPlay();
	auto* pc = GetWorld()->GetFirstPlayerController();
	FInputModeGameAndUI inputMode = FInputModeGameAndUI();
	inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	inputMode.SetHideCursorDuringCapture(false);
	pc->SetShowMouseCursor(true);
	pc->SetInputMode(inputMode);
	
	MouseState = EGameMouseState::Default;

	if (UEnhancedInputLocalPlayerSubsystem* sub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer()))
	{
		sub->AddMappingContext(DefaultContext, 0);	
	}
	
	MouseManager = NewObject<UMouseManager>(this, MouseManagerClass);
	MouseManager->RegisterComponent();

	MouseManager->Initialize();
	
	PlayerUI = Cast<UPlayerUI>(CreateWidget(GetWorld(), PlayerUIClass));
	PlayerUI->AddToViewport();

	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayableCharacterBase::StaticClass(),actors);
	for (auto* a : actors)
	{
		if (auto* cast = Cast<APlayableCharacterBase>(a))
		{
			cast->OnInitialized.AddLambda([cast, this]()
			{
				cast->OnCharacterPrepareAction.Add(FOnCharacterPrepareAction::FDelegate::CreateLambda([this, cast](const UCharacterActionData* data)
				{
					if (data->SkillCase == ESkillCase::SpellOne || data->SkillCase == ESkillCase::SpellTwo)
					{
						this->PlayerUI->ShowCostSpell(cast, data->SkillCase);	
					}
					this->PlayerUI->ShowCost(cast, EActionCase::DefaultAction);
				}));
				cast->OnCharacterPrepareBonusAction.Add(FOnCharacterPrepareBonusAction::FDelegate::CreateLambda([this, cast](const UCharacterActionData* data)
				{
					if (data->SkillCase == ESkillCase::SpellOne || data->SkillCase == ESkillCase::SpellTwo)
					{
						this->PlayerUI->ShowCostSpell(cast, data->SkillCase);	
					}
					this->PlayerUI->ShowCost(cast, EActionCase::BonusAction);
				}));
				cast->OnCharacterAction.Add(FOnCharacterAction::FDelegate::CreateLambda([this, cast](const UCharacterActionData* data)
				{
					if (data->SkillCase == ESkillCase::SpellOne || data->SkillCase == ESkillCase::SpellTwo)
					{
						this->PlayerUI->ShowUsedSpell(cast, data->SkillCase);	
					}
					this->PlayerUI->ShowUsed(cast, EActionCase::DefaultAction);
					this->PlayerUI->ActionListViewer->MoveCursor(0);
					this->MouseManager->SetMouseMode(EGameMouseState::Move);
				}));
				cast->OnCharacterBonusAction.Add(FOnCharacterAction::FDelegate::CreateLambda([this, cast](const UCharacterActionData* data)
				{
					if (data->SkillCase == ESkillCase::SpellOne || data->SkillCase == ESkillCase::SpellTwo)
					{
						this->PlayerUI->ShowUsedSpell(cast, data->SkillCase);	
					}
					this->PlayerUI->ShowUsed(cast, EActionCase::BonusAction);
					this->PlayerUI->ActionListViewer->MoveCursor(0);
					this->MouseManager->SetMouseMode(EGameMouseState::Move);
				}));
				cast->OnTakeDefaultDamage.Add(FOnTakeDefaultDamage::FDelegate::CreateLambda([this, cast]
				(float Damage, AMoveCharacterBase* damagedCharacter, AMoveCharacterBase* instigator)
				{
					this->GetPlayerUI()->ShowSelectedObjectInfo(instigator);
					if (cast->GetIsTurn()) this->GetPlayerUI()->SetSelectedCharacter(cast);
				}));
				if (auto* controller = Cast<AMovableCharacterController>(cast->GetController()))
				{
					controller->OnAIMoveCompleted.Add(FSimpleDelegate::CreateLambda([this, cast]()
					{
						if (cast->GetIsTurn()) this->GetPlayerUI()->SetSelectedCharacter(cast);
					}));
				}
			});

			cast->GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &AMouseControlledPlayer::OnMontageEnded);

			cast->Initialize();
		}
	}

	// 임시 - 월드내의 아무 APlayableCharacterBase 타입의 객체를 가져와서 선택.
	AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayableCharacterBase::StaticClass());

	if (auto* temp = Cast<ISelectableObject>(actor))
	{
		Select(temp);
		Focus(FVector(actor->GetActorLocation().X, actor->GetActorLocation().Y, GetActorLocation().Z));
	}

	if (auto* gm = Cast<ABG3GameMode>(GetWorld()->GetAuthGameMode()))
	{
		gm->Initialize();
	}

}

// Called every frame
void AMouseControlledPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsFocus)
	{
		Direction = ForwardDirection + RightDirection;
		FVector location = GetActorLocation() + Direction.GetSafeNormal() * DeltaTime * MoveSpeed;
		SetActorLocation(location);	
	}
	else
	{
		if (FVector::Distance(GetActorLocation(), FocusPoint) <= 5.f && nullptr == SelectedEnemyCharacter) bIsFocus = false;

		if (nullptr != SelectedEnemyCharacter)
		{
			FocusPoint.X = SelectedEnemyCharacter->GetActorLocation().X;
			FocusPoint.Y = SelectedEnemyCharacter->GetActorLocation().Y;
			FocusPoint.Z = GetActorLocation().Z;
		}
		SetActorLocation(FMath::Lerp(GetActorLocation(), FocusPoint, DeltaTime * 5.f));
	}
	
	SpringArmComp->TargetArmLength = FMath::Lerp(SpringArmComp->TargetArmLength, TargetLength, DeltaTime * 5.f);
	SpringArmComp->SetRelativeRotation(FRotator(FMath::Lerp(0, -47.f, SpringArmComp->TargetArmLength / MaxLength), 0, 0));
	
	if (nullptr != selectedPlayableChar && selectedPlayableChar->GetCurrentMOV() > 0.0f && !selectedPlayableChar->GetIsMoving() && selectedPlayableChar->GetIsTurn())
	{
		auto* pc = GetWorld()->GetFirstPlayerController();
		if (pc)
		{
			FHitResult hit;
			if (pc->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_WorldDynamic), false, hit))
			{
				FVector dest = FVector(hit.ImpactPoint.X, hit.ImpactPoint.Y, hit.ImpactPoint.Z);
				FVector extent = FVector(150.f, 150.f, 200.f);
				if (auto* castChar = Cast<AMoveCharacterBase>(hit.GetActor()))
				{
					dest.Z = hit.GetActor()->GetActorLocation().Z;
					FVector actorBoundsOrigin, ActorBoundsExtent;
					hit.GetActor()->GetActorBounds(true, actorBoundsOrigin, ActorBoundsExtent);
					extent += ActorBoundsExtent;

					if (nullptr != hoverCharacter && hoverCharacter != castChar)
					{
						hoverCharacter->SetOutline(false);
						hoverCharacter = castChar;
						castChar->SetOutline(true);
						PlayerUI->ShowSelectedObjectInfo(hoverCharacter);
					}
					else if (nullptr == hoverCharacter)
					{
						hoverCharacter = castChar;
						hoverCharacter->SetOutline(true);
						PlayerUI->ShowSelectedObjectInfo(hoverCharacter);
					}
				}
				else
				{
					if (nullptr != hoverCharacter)
					{
						hoverCharacter->SetOutline(false);
						hoverCharacter = nullptr;
					}
				}
				if (FVector::Distance(dest, lastCursorPos) < 20.f) return;

				lastCursorPos = dest;
				if (!selectedPlayableChar->GetSplineCondition()) return;
				
				float Distance = selectedPlayableChar->ShowPath(dest, extent);

				if (auto* cursor = MouseManager->GetCursor())
				{
					bool condition = false;
					
					if (auto* castCursor = Cast<UMoveCursor>(cursor))
					{
						if (Distance > 0.01f)
						{
							condition = Distance / 100.f < selectedPlayableChar->GetCurrentMOV();
						}
						
						castCursor->ShowDistance(Distance, condition);

						float curProgress = FMath::Clamp(selectedPlayableChar->GetCurrentMOV() / selectedPlayableChar->GetMaxMov(), 0.0f, 1.0f);
						float resultProgress = FMath::Clamp((selectedPlayableChar->GetCurrentMOV() - Distance / 100.f) / selectedPlayableChar->GetMaxMov(), 0.0f, 1.0f);

						PlayerUI->ShowMoveProgress(curProgress, resultProgress);
					}
					else if (auto* castCursor2 = Cast<UActionCursor>(cursor))
					{
						float curProgress = FMath::Clamp(selectedPlayableChar->GetCurrentMOV() / selectedPlayableChar->GetMaxMov(), 0.0f, 1.0f);
						float resultProgress = FMath::Clamp((selectedPlayableChar->GetCurrentMOV() - Distance / 100.f) / selectedPlayableChar->GetMaxMov(), 0.0f, 1.0f);

						PlayerUI->ShowMoveProgress(curProgress, resultProgress);
					}
					else
					{
						PlayerUI->ShowMoveProgress(0, 0);
					}
				}
			}
		}
	}
}

void AMouseControlledPlayer::SetFocusEnemy(class ANonPlayableCharacterBase* character)
{
	SelectedEnemyCharacter = character;
}

UMouseManager* AMouseControlledPlayer::GetMouseManager() const
{
	return MouseManager;	
}

APlayableCharacterBase* AMouseControlledPlayer::GetPlayableCharacter() const
{
	return selectedPlayableChar;
}

class UPlayerUI* AMouseControlledPlayer::GetPlayerUI() const
{
	return PlayerUI;
}

void AMouseControlledPlayer::Focus(FVector focusLocation)
{
	bIsFocus = true;
	FocusPoint = focusLocation;
}

void AMouseControlledPlayer::Select(ISelectableObject* selectedObject)
{
	if (nullptr != SelectedObject)
	{
		SelectedObject->Deselected();
	}
	SelectedObject = selectedObject;
	SelectedObject->Selected();

	// 임시로 움직이는 캐릭터일 경우에만 정보 받아올 수 있도록 적용, 나중에 선택할 수 있는 오브젝트면 다 표시해주어야함.
	if (auto* cast = Cast<AMoveCharacterBase>(SelectedObject))
	{
		PlayerUI->ShowSelectedObjectInfo(cast);
	}

	if (auto* playable = Cast<APlayableCharacterBase>(SelectedObject))
	{
		selectedPlayableChar = playable;
		// 선택한 오브젝트가 플레이어 캐릭터일 경우, 캐릭터를 PlayerUI에 넣어줌.
		PlayerUI->SetSelectedCharacter(selectedPlayableChar);
					
		if (selectedPlayableChar->MovablePtr && selectedPlayableChar->GetIsTurn()) MouseManager->SetMouseMode(EGameMouseState::Move);
		else MouseManager->SetMouseMode(EGameMouseState::Default);

		Focus(FVector(selectedPlayableChar->GetActorLocation().X, selectedPlayableChar->GetActorLocation().Y, GetActorLocation().Z));
	}
	else
	{
		selectedPlayableChar = nullptr;
		MouseManager->SetMouseMode(EGameMouseState::Default);
		PlayerUI->ShowMoveProgress(0,0);
	}
}

#pragma region Input
// Called to bind functionality to input
void AMouseControlledPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this, &AMouseControlledPlayer::OnLeftMouseButtonDown);
		EnhancedInputComponent->BindAction(RightClickAction, ETriggerEvent::Started, this, &AMouseControlledPlayer::OnRightMouseButtonDown);
		EnhancedInputComponent->BindAction(OutlineAction, ETriggerEvent::Triggered, this, &AMouseControlledPlayer::OnAltTriggered);
		EnhancedInputComponent->BindAction(OutlineAction, ETriggerEvent::Completed, this, &AMouseControlledPlayer::OnAltComplete);
		
		EnhancedInputComponent->BindAction(ForwardAction, ETriggerEvent::Started, this, &AMouseControlledPlayer::StartForwardMove);
		EnhancedInputComponent->BindAction(ForwardAction, ETriggerEvent::Triggered, this, &AMouseControlledPlayer::ForwardMove);
		EnhancedInputComponent->BindAction(ForwardAction, ETriggerEvent::Completed, this, &AMouseControlledPlayer::StopForwardMove);
		
		EnhancedInputComponent->BindAction(BackAction, ETriggerEvent::Started, this, &AMouseControlledPlayer::StartBackMove);
		EnhancedInputComponent->BindAction(BackAction, ETriggerEvent::Triggered, this, &AMouseControlledPlayer::BackMove);
		EnhancedInputComponent->BindAction(BackAction, ETriggerEvent::Completed, this, &AMouseControlledPlayer::StopBackMove);
		
		EnhancedInputComponent->BindAction(LeftAction, ETriggerEvent::Started, this, &AMouseControlledPlayer::StartLeftMove);
		EnhancedInputComponent->BindAction(LeftAction, ETriggerEvent::Triggered, this, &AMouseControlledPlayer::LeftMove);
		EnhancedInputComponent->BindAction(LeftAction, ETriggerEvent::Completed, this, &AMouseControlledPlayer::StopLeftMove);
		
		EnhancedInputComponent->BindAction(RightAction, ETriggerEvent::Started, this, &AMouseControlledPlayer::StartRightMove);
		EnhancedInputComponent->BindAction(RightAction, ETriggerEvent::Triggered, this, &AMouseControlledPlayer::RightMove);
		EnhancedInputComponent->BindAction(RightAction, ETriggerEvent::Completed, this, &AMouseControlledPlayer::StopRightMove);

		EnhancedInputComponent->BindAction(UpAction, ETriggerEvent::Started, this, &AMouseControlledPlayer::StartRotateRight);
		EnhancedInputComponent->BindAction(UpAction, ETriggerEvent::Triggered, this, &AMouseControlledPlayer::RotateRight);
		EnhancedInputComponent->BindAction(UpAction, ETriggerEvent::Completed, this, &AMouseControlledPlayer::StopRotateRight);
		
		EnhancedInputComponent->BindAction(DownAction, ETriggerEvent::Started, this, &AMouseControlledPlayer::StartRotateLeft);
		EnhancedInputComponent->BindAction(DownAction, ETriggerEvent::Triggered, this, &AMouseControlledPlayer::RotateLeft);
		EnhancedInputComponent->BindAction(DownAction, ETriggerEvent::Completed, this, &AMouseControlledPlayer::StopRotateLeft);

		EnhancedInputComponent->BindAction(WheelAction, ETriggerEvent::Triggered, this, &AMouseControlledPlayer::Wheel);
	}
}

void AMouseControlledPlayer::OnLeftMouseButtonDown()
{
	auto* pc = GetWorld()->GetFirstPlayerController();
	
	if (pc)
	{
		FHitResult hit;
		// 클릭한 곳이 바닥일 경우, 현재 선택한 오브젝트가 존재하고, 그 캐릭터가 move가 가능하다면 클릭한 지점으로 이동

		if (pc->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_WorldDynamic), false, hit))
		{
			if (nullptr != selectedPlayableChar && selectedPlayableChar->GetIsTurn())
			{
				if (auto* castCursor = Cast<UActionCursor>(MouseManager->GetCursor()))
				{
					FVector extent = FVector(150.f, 150.f, 200.f);
					FVector destination = lastCursorPos;
					float Distance = selectedPlayableChar->ShowPath(destination, extent);
					selectedPlayableChar->SetSplineCondition(false);
					
					if (auto* multiCursor = Cast<UMultiTargetActionCursor>(castCursor))
					{
						if (auto* castChar = Cast<AMoveCharacterBase>(hit.GetActor()))
						{
							if (multiCursor->GetAction()->MaxDistance < FVector::Distance(selectedPlayableChar->GetActorLocation(), castChar->GetActorLocation()) / 100.f) return;
							FVector actorBoundsOrigin, ActorBoundsExtent;
							castChar->GetActorBounds(true, actorBoundsOrigin, ActorBoundsExtent);
							extent += ActorBoundsExtent;

							int tempTargetNum = multiCursor->GetAction()->UpcastNum > 2 ? *castChar->CurHPPtr : 1;

							if (multiCursor->CurTargetNum + tempTargetNum > multiCursor->GetAction()->CurMaxTargetCount) return;

							multiCursor->GetAction()->Target.Add(castChar);
							multiCursor->ShowTargetProgress(tempTargetNum);
							
							if (multiCursor->CurTargetNum == multiCursor->GetAction()->CurMaxTargetCount)
							{
								if (auto* gm = Cast<ABG3GameMode>(GetWorld()->GetAuthGameMode()))
								{
									gm->ActionManager->ExecuteAction(multiCursor->GetAction(), selectedPlayableChar);	
								}
							}
						}
						return;
					}
					
					castCursor->GetAction()->Target.Empty();
					
					if (castCursor->GetAction()->SkillCase == ESkillCase::Buff)
					{
						if (auto* gm = Cast<ABG3GameMode>(GetWorld()->GetAuthGameMode()))
						{
							if (castCursor->GetAction()->MaxDistance < 0.05f)
							{
								gm->ActionManager->ExecuteAction(castCursor->GetAction(), selectedPlayableChar);
								return;
							}
						}
					}

					if (auto* castChar = Cast<AMoveCharacterBase>(hit.GetActor()))
					{
						FVector actorBoundsOrigin, ActorBoundsExtent;
						castChar->GetActorBounds(true, actorBoundsOrigin, ActorBoundsExtent);
						extent += ActorBoundsExtent;
						destination = castChar->GetActorLocation();
						
						if (auto* playableCharacter = Cast<APlayableCharacterBase>(castChar))
						{
							if (playableCharacter != selectedPlayableChar) castCursor->GetAction()->Target.Add(castChar);
							else return;
						}
						else
						{
							castCursor->GetAction()->Target.Add(castChar);
						}
					}
					
					Distance = selectedPlayableChar->ShowPath(destination, extent);
					
					if (selectedPlayableChar->GetCurrentMOV() + castCursor->GetAction()->MaxDistance < Distance / 100.f) return;
					if (selectedPlayableChar->GetIsNoPath() && castCursor->GetAction()->MaxDistance < 2.1f) return;
					
					if (auto* gm = Cast<ABG3GameMode>(GetWorld()->GetAuthGameMode()))
					{
						if ((FVector::Distance(selectedPlayableChar->GetActorLocation(), destination) - extent.X) / 100.f <= castCursor->GetAction()->MaxDistance)
						{
							UE_LOG(LogTemp,Warning,TEXT("Execute"));
							
							FVector dir = destination - selectedPlayableChar->GetActorLocation();
							dir.Z = 0;
							FRotator rotation = dir.Rotation();
							FQuat temp = rotation.Quaternion() * selectedPlayableChar->GetActorRotation().Quaternion().Inverse();

							TArray<AActor*> actors;

							selectedPlayableChar->GetAttachedActors(actors);
							selectedPlayableChar->SetActorRotation(rotation);
							
							for (auto* actor : actors)
							{
								if (auto* cast = Cast<AParabolaSpline>(actor))
								{
									FQuat splineQuat = cast->GetActorQuat();
									FQuat newSplineQuat = temp.Inverse() * splineQuat;
									cast->SetActorRotation(newSplineQuat);
								}
							}
							gm->ActionManager->ExecuteAction(castCursor->GetAction(), selectedPlayableChar);
						}
						else
						{
							UE_LOG(LogTemp,Warning,TEXT("MoveExecute"));
							selectedPlayableChar->ExecuteAction(gm, castCursor->GetAction(), destination);
						}
						return;
					}
				}

				else if (auto* castCursor2 = Cast<UJumpCursor>(MouseManager->GetCursor()))
				{
					if (auto* gm = Cast<ABG3GameMode>(GetWorld()->GetAuthGameMode()))
					{
						if (FVector::DistXY(GetPlayableCharacter()->GetActorLocation(), hit.ImpactPoint) / 100.f > castCursor2->GetMaxDistance()) return;
						float fallingDistance = GetPlayableCharacter()->GetActorLocation().Z - hit.ImpactPoint.Z;
						if (fallingDistance / 100.f <= -castCursor2->GetMaxDistance()) return;

						TArray<AActor*> actors;
						GetPlayableCharacter()->GetAttachedActors(actors);
						for (auto* actor : actors)
						{
							if (auto* cast = Cast<AParabolaSpline>(actor))
							{
								if (cast->GetIsBlocked()) return;
							}
						}

						gm->ActionManager->ExecuteAction(castCursor2->GetActionData(), selectedPlayableChar);
						return;
					}
					return;
				}
			}
			
			// HitActor가 선택할 수 있는 물체일경우
			// Selected 함수 실행
			// 이후 현재 선택된 오브젝트 변수에 담아둠
			if (auto* actor = Cast<ISelectableObject>(hit.GetActor()))
			{
				Select(actor);
				return;
			}
			
			if (nullptr == selectedPlayableChar) return;
			if (*(selectedPlayableChar->MovablePtr) && selectedPlayableChar->GetCurrentMOV() > 0.0f)
			{
				selectedPlayableChar->SetDestination(lastCursorPos);
				//UE_LOG(LogTemp, Warning, TEXT("Destination : %f, %f, %f"), hit.ImpactPoint.X, hit.ImpactPoint.Y, selectedPlayableChar->GetActorLocation().Z);
				selectedPlayableChar->Move();
			}
		}
	}
}

void AMouseControlledPlayer::OnAltTriggered()
{
	if (bIsOutline) return;

	bIsOutline = true;
	if (auto* gm = Cast<ABG3GameMode>(GetWorld()->GetAuthGameMode()))
	{
		gm->SetAllCharacterOutline(true);
	}
}

void AMouseControlledPlayer::OnAltComplete()
{
	bIsOutline = false;
	if (auto* gm = Cast<ABG3GameMode>(GetWorld()->GetAuthGameMode()))
	{
		gm->SetAllCharacterOutline(false);
	}

	if (SelectedObject)
	{
		SelectedObject->Selected();	
	}
}

void AMouseControlledPlayer::OnRightMouseButtonDown()
{
	if (selectedPlayableChar == nullptr) return;
	if (!selectedPlayableChar->GetIsTurn()) return;
	
	MouseManager->SetMouseMode(EGameMouseState::Move);
	// 사용 코스트 표시 취소
	PlayerUI->SetSelectedCharacter(GetPlayableCharacter());
	selectedPlayableChar->StopAction();
}

void AMouseControlledPlayer::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	MouseManager->SetMouseMode(EGameMouseState::Move);
}

void AMouseControlledPlayer::StartForwardMove(const FInputActionValue& value)
{
	bIsforward = true;
	bIsback = false;
}

void AMouseControlledPlayer::StartBackMove(const FInputActionValue& value)
{
	bIsback = true;
	bIsforward = false;
}

void AMouseControlledPlayer::StartLeftMove(const FInputActionValue& value)
{
	bIsLeft = true;
	bIsRight = false;
}

void AMouseControlledPlayer::StartRightMove(const FInputActionValue& value)
{
	bIsRight = true;
	bIsLeft = false;
}

void AMouseControlledPlayer::StartRotateRight(const FInputActionValue& value)
{
	bIsUp = true;
	bIsDown = false;
}

void AMouseControlledPlayer::StartRotateLeft(const FInputActionValue& value)
{
	bIsDown = true;
	bIsUp = false;
}

void AMouseControlledPlayer::ForwardMove(const FInputActionValue& value)
{
	if (bIsback) return;
	
	FVector2D v = value.Get<FVector2D>();

	FRotator YawRotation(0.f, GetActorRotation().Yaw, 0.f);
	ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) * v.X;
}

void AMouseControlledPlayer::StopForwardMove(const FInputActionValue& value)
{
	bIsforward = false;
	FVector2D v = value.Get<FVector2D>();
	
	FRotator YawRotation(0.f, GetActorRotation().Yaw, 0.f);
	ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) * v.X;
}

void AMouseControlledPlayer::BackMove(const FInputActionValue& value)
{
	if (bIsforward) return;
	
	FVector2D v = value.Get<FVector2D>();
	
	FRotator YawRotation(0.f, GetActorRotation().Yaw, 0.f);
	ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) * v.X;
}

void AMouseControlledPlayer::StopBackMove(const FInputActionValue& value)
{
	bIsback = false;
	FVector2D v = value.Get<FVector2D>();
	
	FRotator YawRotation(0.f, GetActorRotation().Yaw, 0.f);
	ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) * v.X;
}

void AMouseControlledPlayer::LeftMove(const FInputActionValue& value)
{
	if (bIsRight) return;
	
	FVector2D v = value.Get<FVector2D>();
	
	FRotator YawRotation(0.f, GetActorRotation().Yaw, 0.f);
	RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) * v.Y;
}

void AMouseControlledPlayer::StopLeftMove(const FInputActionValue& value)
{
	bIsLeft = false;
	FVector2D v = value.Get<FVector2D>();
	FRotator YawRotation(0.f, GetActorRotation().Yaw, 0.f);
	RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) * v.Y;
}

void AMouseControlledPlayer::RightMove(const FInputActionValue& value)
{
	if (bIsLeft) return;
	
	FVector2D v = value.Get<FVector2D>();
	FRotator YawRotation(0.f, GetActorRotation().Yaw, 0.f);
	RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) * v.Y;
}

void AMouseControlledPlayer::StopRightMove(const FInputActionValue& value)
{
	bIsRight = false;
	FVector2D v = value.Get<FVector2D>();
	FRotator YawRotation(0.f, GetActorRotation().Yaw, 0.f);
	RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) * v.Y;
}


void AMouseControlledPlayer::RotateRight(const FInputActionValue& value)
{
	if (bIsDown) return;
	
	FVector v = value.Get<FVector>();
	
	AddActorWorldRotation(FRotator(0, v.Z, 0));
}

void AMouseControlledPlayer::RotateLeft(const FInputActionValue& value)
{
	if (bIsUp) return;
	
	FVector v = value.Get<FVector>();
	AddActorWorldRotation(FRotator(0, v.Z, 0));
}

void AMouseControlledPlayer::StopRotateRight(const FInputActionValue& value)
{
	bIsUp = false;
	FVector v = value.Get<FVector>();
	Direction.Z = v.Z;
}

void AMouseControlledPlayer::StopRotateLeft(const FInputActionValue& value)
{
	bIsDown = false;
	FVector v = value.Get<FVector>();
	Direction.Z = v.Z;
}

void AMouseControlledPlayer::Wheel(const FInputActionValue& value)
{
	float v = value.Get<float>();

	TargetLength = FMath::Clamp(SpringArmComp->TargetArmLength + v * WheelSpeed, MinLength, MaxLength);
}

void AMouseControlledPlayer::InitializeInput()
{
	ConstructorHelpers::FObjectFinder<UInputMappingContext> defaultContext(TEXT("/Game/BG3/Input/BG3InputMappingContext.BG3InputMappingContext"));

	if (defaultContext.Succeeded())
	{
		DefaultContext = defaultContext.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> clickAction(TEXT("/Game/BG3/Input/Action/IA_LeftClick.IA_LeftClick"));
	if (clickAction.Succeeded())
	{
		ClickAction = clickAction.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> forwardAction(TEXT("/Game/BG3/Input/Action/IA_Forward.IA_Forward"));

	if (forwardAction.Succeeded())
	{
		ForwardAction = forwardAction.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> backAction(TEXT("/Game/BG3/Input/Action/IA_Back.IA_Back"));

	if (backAction.Succeeded())
	{
		BackAction = backAction.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> leftAction(TEXT("/Game/BG3/Input/Action/IA_Left.IA_Left"));

	if (leftAction.Succeeded())
	{
		LeftAction = leftAction.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> rightAction(TEXT("/Game/BG3/Input/Action/IA_Right.IA_Right"));

	if (rightAction.Succeeded())
	{
		RightAction = rightAction.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> upAction(TEXT("/Game/BG3/Input/Action/IA_UP.IA_UP"));

	if (upAction.Succeeded())
	{
		UpAction = upAction.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> downAction(TEXT("/Game/BG3/Input/Action/IA_Down.IA_Down"));

	if (downAction.Succeeded())
	{
		DownAction = downAction.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> wheelAction(TEXT("/Game/BG3/Input/Action/IA_MouseWheel.IA_MouseWheel"));

	if (wheelAction.Succeeded())
	{
		WheelAction = wheelAction.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> outlineAction(TEXT("/Game/BG3/Input/Action/IA_ShowOutline.IA_ShowOutline"));

	if (outlineAction.Succeeded())
	{
		OutlineAction = outlineAction.Object;
	}

	ConstructorHelpers::FObjectFinder<UInputAction> rightClickAction(TEXT("/Script/EnhancedInput.InputAction'/Game/BG3/Input/Action/IA_RightClick.IA_RightClick'"));
	
	if (rightClickAction.Succeeded())
	{
		RightClickAction = rightClickAction.Object;
	}
}

#pragma endregion

