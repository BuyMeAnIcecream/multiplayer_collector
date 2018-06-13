// Fill out your copyright notice in the Description page of Project Settings.

#include "BatteryPickup.h"
#include "Components/StaticMeshComponent.h"

ABatteryPickup::ABatteryPickup() {

	//sync server and client movement
	bReplicateMovement = true;
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	GetStaticMeshComponent()->SetSimulatePhysics(true);

}

void ABatteryPickup::WasCollected_Implementation() {

	//allow parent class to handle FIRST
	Super::WasCollected_Implementation();
	//Destroy the battery
	Destroy();

}
