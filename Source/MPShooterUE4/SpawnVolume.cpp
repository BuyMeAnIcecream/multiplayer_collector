// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "TimerManager.h"
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>


// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	if (Role == ROLE_Authority)
	{

		WhereToSpawn = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
		RootComponent = WhereToSpawn;

		SpawnDelayRangeLow = 1.0f;
		SpawnDelayRangeHigh = 4.5f;
	}
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASpawnVolume::GetRandomPointInVolume()
{
	if (WhereToSpawn != NULL)
	{
		FVector SpawnOrigin = WhereToSpawn->Bounds.Origin;
		FVector SpawnExtent = WhereToSpawn->Bounds.BoxExtent;
		return UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtent);
	}
	return FVector();
	
}

void ASpawnVolume::SetSpawningActive(bool bShouldSpawn)
{
//	UE_LOG(LogTemp, Warning, TEXT("Bool value is: %s"), bShouldSpawn ? TEXT("true") : TEXT("false"));
//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("This is an on screen message!"));
	if(Role == ROLE_Authority)
		
		if (bShouldSpawn)
		{
			//start spawning
			//set timer to start spawning pickups
			SpawnDelay = FMath::FRandRange(SpawnDelayRangeLow, SpawnDelayRangeHigh);
			GetWorldTimerManager().SetTimer(SpawnTimer, this, &ASpawnVolume::SpawnPickup, SpawnDelay, false);
		}
		else
		{
			//stop spawning
			GetWorldTimerManager().ClearTimer(SpawnTimer);
		}
}

void ASpawnVolume::SpawnPickup()
{
	if (Role == ROLE_Authority && WhatToSpawn != NULL)
	{
		//check fo valid world
		if (UWorld* const World = GetWorld())
		{
			//setup any params
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;

			FVector SpawnLocation = GetRandomPointInVolume();

			FRotator SpawnRotation;
			SpawnRotation.Yaw = FMath::FRand() * 360.f;
			SpawnRotation.Pitch = FMath::FRand() * 360.f;
			SpawnRotation.Roll = FMath::FRand() * 360.f;

			APickup* const SpawnedPickup = World->SpawnActor<APickup>(WhatToSpawn, SpawnLocation, SpawnRotation, SpawnParams);
			
			//Delay before spawning another coke
			SpawnDelay = FMath::FRandRange(SpawnDelayRangeLow, SpawnDelayRangeHigh);
			GetWorldTimerManager().SetTimer(SpawnTimer, this, &ASpawnVolume::SpawnPickup, SpawnDelay, false);

			SetSpawningActive(true);
		}
	}
}

