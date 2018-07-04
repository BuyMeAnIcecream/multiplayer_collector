// Fill out your copyright notice in the Description page of Project Settings.

#include "Banana.h"

ABanana::ABanana() 
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;
	GetStaticMeshComponent()->bGenerateOverlapEvents = true;

	//sync server and client movement
	bReplicateMovement = true;
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	GetStaticMeshComponent()->SetSimulatePhysics(true);

}

void ABanana::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABanana, bIsActive);
}

void ABanana::KnockOut_Implementation()
{
	//TODO add implementation in 3rdCharacter

	UE_LOG(LogClass, Log, TEXT("APickup::WasCollected_Implementation() %s"), *GetName());
}

bool ABanana::IsActive() {
	return bIsActive;
}
