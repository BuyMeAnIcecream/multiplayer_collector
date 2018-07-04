 // Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CollectorGameState.h"
#include "MPShooterUE4GameMode.generated.h"

UCLASS(minimalapi)
class AMPShooterUE4GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMPShooterUE4GameMode();

	virtual void BeginPlay() override;
	//Decay rate getter
	UFUNCTION(BlueprintPure, Category= "Power")
	float GetDecayRate();

	//Access the power level required to win the game
	UFUNCTION(BlueprintPure, Category = "Power")
	float GetPowerToWinMultiplier();

protected:
	//how many times per sec update character's power 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Power")
	float PowerDrainDelay;

	//Access the timer for recuring power drain
	FTimerHandle PowerDrainTimer;

	//Rate of powerloss (%initial power / sec)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float DecayRate;

	//The power level needed to win
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float PowerToWinMultiplier;
	
	//Track number of players who ran out of energy and were eliminated
	int32 DeadPlayerCount;

	//To communicate with games GameState
	ACollectorGameState* MyGameState;

private:

	//Drain power here
	void DrainPowerOverTime();

	//Stores all the spawn vols
	TArray<class ASpawnVolume*> SpawnVolumeActors;

	//Handle any function calls for when the game transitions between states
	void HandleNewState(EBatteryPlayState NewState);

	//Debug
	void Log();
};



