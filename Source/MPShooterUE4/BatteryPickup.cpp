// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "BatteryPickup.h"
#include "Components/StaticMeshComponent.h"

ABatteryPickup::ABatteryPickup() {

	//sync server and client movement
	bReplicateMovement = true;
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	GetStaticMeshComponent()->SetSimulatePhysics(true);

}


void ABatteryPickup::PickedUpBy(APawn * Pawn)
{
	Super::PickedUpBy(Pawn);
	if (Role == ROLE_Authority)
	{
		//delay deletion to give client time to play vfx etc
		SetLifeSpan(2.0f);
	}
}
