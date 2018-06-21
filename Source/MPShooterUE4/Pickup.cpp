// Fill out your copyright notice in the Description page of Project Settings.

#include "Pickup.h"


APickup::APickup() {

	//enabling replication
	bReplicates = true;

	PrimaryActorTick.bCanEverTick = false;

	// stat component's overlap is off by default
	GetStaticMeshComponent()->bGenerateOverlapEvents = true;

	if (Role == ROLE_Authority) {
		bIsActive = true;
	}
}

void APickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APickup, bIsActive);
}

bool APickup::IsActive() {
	return bIsActive;
}

void APickup::SetActive(bool NewPickupState) {
	if (Role == ROLE_Authority)
	{
		bIsActive = NewPickupState;
	}
}

void APickup::OnRep_IsActive()
{
	
}



void APickup::WasCollected_Implementation()
{
	UE_LOG(LogClass, Log, TEXT("APickup::WasCollected_Implementation() %s"), *GetName());
}




void APickup::PickedUpBy(APawn* Pawn)
{
	if (Role == ROLE_Authority)
	{
		PickupInstigator = Pawn;
		//Notify clients of the picked up action
		ClientOnPickedUpBy(Pawn);
	}
}

void APickup::ClientOnPickedUpBy_Implementation(APawn* Pawn)
{
	//store instigator
	PickupInstigator = Pawn;
	//fire bp native event, which can't be replicated
	WasCollected();
}