 // Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MPShooterUE4GameMode.generated.h"

UCLASS(minimalapi)
class AMPShooterUE4GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMPShooterUE4GameMode();

	virtual void BeginPlay() override;

	//decay rate getter
	UFUNCTION(BlueprintPure, Category= "Power")
	float GetDecayRate();

	//how manye times per sec update character's power 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Power")
	float PowerDrainDelay;

	//Access the timer for recuring power drain
	FTimerHandle PowerDrainTimer;
protected:
	//rate of powerloss (%initial power / sec)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float DecayRate;

private:

	//drain power here
	void DrainPowerOverTime();
};



