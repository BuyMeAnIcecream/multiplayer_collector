// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "BatteryPickup.h"
#include "Components/StaticMeshComponent.h"

ABatteryPickup::ABatteryPickup() {

	//sync server and client movement
	bReplicateMovement = true;
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	GetStaticMeshComponent()->SetSimulatePhysics(true);

	//set base val of batteryPower
	BatteryPower = 200.0f;
}


void ABatteryPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABatteryPickup, BatteryPower);
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

float ABatteryPickup::GetPower()
{
	return BatteryPower;
}
