// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "MPShooterUE4GameMode.h"
#include "MPShooterUE4Character.h"
//#include "Runtime/Engine/Classes/GameFramework/Actor.h"
//#include "Runtime/Engine/Classes/Engine/World.h"
#include "UObject/ConstructorHelpers.h"


AMPShooterUE4GameMode::AMPShooterUE4GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	//base value decayRate
	DecayRate = 0.02f;

	//base value for how freq to drain
	PowerDrainDelay = 0.25f;
}

void AMPShooterUE4GameMode::BeginPlay()
{
	GetWorldTimerManager().SetTimer(PowerDrainTimer, this, &AMPShooterUE4GameMode::DrainPowerOverTime, PowerDrainDelay, true);

}

float AMPShooterUE4GameMode::GetDecayRate()
{
	return DecayRate;
}

void AMPShooterUE4GameMode::DrainPowerOverTime() 
{
	//Access world to get to players
	UWorld* World = GetWorld();
	check(World);

	//loop over characters and drain their power
	for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(*It))
		{
			if (AMPShooterUE4Character* BatteryCharacter = Cast<AMPShooterUE4Character>(PlayerController->GetPawn()))
			{
				if (BatteryCharacter->GetCurrentPower() > 0)
				{
					BatteryCharacter->UpdatePower(-PowerDrainDelay*DecayRate*(BatteryCharacter->GetInitialPower()));
				}
			}
		}
	}
}