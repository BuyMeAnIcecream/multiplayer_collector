// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "MPShooterUE4GameMode.h"
#include "MPShooterUE4Character.h"
#include "GameFramework/HUD.h"
#include "string"
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
	Super::BeginPlay();
	

	UWorld* World = GetWorld();
	check(World);
	MyGameState = Cast<ACollectorGameState>(GameState);
	check(MyGameState);

	//reset stats
	DeadPlayerCount = 0;

	//Gather all the spawn volumes
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, ASpawnVolume::StaticClass(), FoundActors);

	
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

	//timer for debuggin logs
//	FTimerHandle DebugLogTimer;
//	GetWorldTimerManager().SetTimer(DebugLogTimer, this, &AMPShooterUE4GameMode::Log, 5.0f, true);

	//transitioning the game to the playing state
	HandleNewState(EBatteryPlayState::EPlaying);
}

//debug_print from loop
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
					MyGameState->WinningPlayerName = BatteryCharacter->GetName();
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
					BatteryCharacter->OnPlayerDeath();
					++DeadPlayerCount;
					
					//see if this is the last player to die.
					if (DeadPlayerCount >= GetNumPlayers())
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
//		UE_LOG(LogTemp, Warning, TEXT("UseEnumValue as string : %s"), *GETENUMSTRING("EBatteryPlayState", MyGameState->GetCurrentState()));
		switch (NewState)
		{
		case EBatteryPlayState::EPlaying:
			for (ASpawnVolume* SpawnVol : SpawnVolumeActors)
			{
				SpawnVol->SetSpawningActive(true);
			}
			//Start draining power
			GetWorldTimerManager().SetTimer(PowerDrainTimer, this, &AMPShooterUE4GameMode::DrainPowerOverTime, PowerDrainDelay, true);
//			UE_LOG(LogTemp, Warning, TEXT("Playin"));
			break;

		case EBatteryPlayState::EGameOver:
			for (ASpawnVolume* SpawnVol : SpawnVolumeActors)
			{
				SpawnVol->SetSpawningActive(false);

			}
			//stop draining power
			GetWorldTimerManager().ClearTimer(PowerDrainTimer);

//			UE_LOG(LogTemp, Warning, TEXT("Over"));
			break;

		case EBatteryPlayState::EWon:
			for (ASpawnVolume* SpawnVol : SpawnVolumeActors)
			{
				SpawnVol->SetSpawningActive(false);
			}
//			UE_LOG(LogTemp, Warning, TEXT("Won"));
			break;

		default:
		case EBatteryPlayState::EUnknown:
			break;

		}
	}

}


void AMPShooterUE4GameMode::Log()
{
	//std::string 
	int val = MyGameState->GetCurrentState();
	//
	//std::string str = "State: " + std::to_string(val);//FString::FromInt(val);
	UE_LOG(LogTemp, Warning, TEXT("State: %d"), val);
}

