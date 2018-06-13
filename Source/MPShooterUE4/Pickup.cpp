// Fill out your copyright notice in the Description page of Project Settings.

#include "Pickup.h"
#include "Net/UnrealNetwork.h"

APickup::APickup() {

	//enabling replication
	bReplicates = true;

	PrimaryActorTick.bCanEverTick = false;

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

void APickup::WasCollected()
{

}


void APickup::WasCollected_Implementation()
{
	UE_LOG(LogClass, Log, TEXT("APickup::WasCollected_Implementation() %s"), *GetName());
}