// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BG3Struct.h"
#include "BattleTurnManager.generated.h"


UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BG3CPP_API UBattleTurnManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBattleTurnManager();

	void Initialize();
	
	void StartBattle();

	void SetOutlineAllBattleCharacters(bool condition);

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTurnListViewer> TurnListClass;

	UPROPERTY()
	class UTurnListViewer* TurnList;
	UPROPERTY()
	class AMouseControlledPlayer* Player;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	TArray<FCharacterTurnData> Characters;
	UPROPERTY()
	TArray<class AMoveCharacterBase*> SortedCharacters;

	TArray<class AMoveCharacterBase*> SortCharacters();
};
