// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "MPShooterUE4GameMode.h"
#include "MPShooterUE4Character.h"
#include "GameFramework/HUD.h"

#include "Kismet/GameplayStatics.h"
//#include "Runtime/Engine/Classes/GameFramework/Actor.h"
//#include "Runtime/Engine/Classes/Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "SpawnVolume.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"


AMPShooterUE4GameMode::AMPShooterUE4GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	
	//set the type of HUD used in the game
	static ConstructorHelpers::FClassFinder<AHUD> PlayerHUDClass(TEXT("/Game/Blueprints/BP_CollectorHUD"));
	if (PlayerHUDClass.Class != NULL)
	{
		HUDClass = PlayerHUDClass.Class;
	}

	//set the type of gamestate used in the game
	GameStateClass = ACollectorGameState::StaticClass();

	//base value decayRate
	DecayRate = 0.02f;

	//base value for how freq to drain
	PowerDrainDelay = 0.25f;

	//set base value for the power multiplier
	PowerToWinMultiplier = 1.25f;

	//reset stats
	DeadPlayerCount = 0;
}

void AMPShooterUE4GameMode::BeginPlay()
{
	GetWorldTimerManager().SetTimer(PowerDrainTimer, this, &AMPShooterUE4GameMode::DrainPowerOverTime, PowerDrainDelay, true);

	UWorld* World = GetWorld();
	check(World);
	ACollectorGameState* MyGameState = Cast<ACollectorGameState>(GameState);
	check(MyGameState);

	//reset stats
	DeadPlayerCount = 0;

	//Gather all the spawn volumes
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, ASpawnVolume::StaticClass(), FoundActors);

	//transitioning the game to the playing state
	HandleNewState(EBatteryPlayState::EPlaying);;
	for (auto Actor:FoundActors)
	{
		if (ASpawnVolume* TestSpawnVol = Cast<ASpawnVolume>(Actor))
		{
			SpawnVolumeActors.AddUnique(TestSpawnVol);
		}
	}
	//loop over characters and drain their power
	for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(*It))
		{
			if (AMPShooterUE4Character* BatteryCharacter = Cast<AMPShooterUE4Character>(PlayerController->GetPawn()))
			{
				MyGameState->PowerToWin = BatteryCharacter->GetInitialPower() * PowerToWinMultiplier;
				break;
			}
		}
	}
}


float AMPShooterUE4GameMode::GetDecayRate()
{
	return DecayRate;
}

float AMPShooterUE4GameMode::GetPowerToWinMultiplier()
{
	return PowerToWinMultiplier;
}

void AMPShooterUE4GameMode::DrainPowerOverTime() 
{
	//Access world to get to players
	UWorld* World = GetWorld();
	check(World);

	ACollectorGameState* MyGameState = Cast<ACollectorGameState>(GameState);
	check(MyGameState);

	//loop over characters and drain their power
	for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(*It))
		{
			if (AMPShooterUE4Character* BatteryCharacter = Cast<AMPShooterUE4Character>(PlayerController->GetPawn()))
			{
				if (BatteryCharacter->GetCurrentPower() > MyGameState->PowerToWin)
				{
					HandleNewState(EBatteryPlayState::EWon);
					//MyGameState->SetCurrentState(EBatteryPlayState::EWon);
				}
				else if (BatteryCharacter->GetCurrentPower() > 0)
				{
					BatteryCharacter->UpdatePower(-PowerDrainDelay*DecayRate*(BatteryCharacter->GetInitialPower()));
				}
				
				else
				{
					//dude you dead
					BatteryCharacter->DetachFromControllerPendingDestroy();
					++DeadPlayerCount;
					
					//see if this is the last player to die.
					if (DeadPlayerCount >= GetNumPlayers() - 1)
					{
						MyGameState->SetCurrentState(EBatteryPlayState::EGameOver);
					//	GetNumPlayers
					}
				}

			}
		}
	}
}

void AMPShooterUE4GameMode::HandleNewState(EBatteryPlayState NewState)
{
	UWorld* World = GetWorld();
	check(World);
	ACollectorGameState* MyGameState = Cast<ACollectorGameState>(GameState);
	check(MyGameState);

	//only transition if this is a new state
	if (NewState != MyGameState->GetCurrentState())
	{
		MyGameState->SetCurrentState(NewState);

		switch (NewState)
		{
		case EBatteryPlayState::EPlaying:
			for (ASpawnVolume* SpawnVol : SpawnVolumeActors)
			{
				SpawnVol->SetSpawningActive(true);
			}
			break;

		case EBatteryPlayState::EGameOver:
			for (ASpawnVolume* SpawnVol : SpawnVolumeActors)
			{
				SpawnVol->SetSpawningActive(true);
			}
			break;

		case EBatteryPlayState::EWon:
			for (ASpawnVolume* SpawnVol : SpawnVolumeActors)
			{
				SpawnVol->SetSpawningActive(false);
			}
			break;

		default:
		case EBatteryPlayState::EUnknown:
			break;

		}
	}

}
