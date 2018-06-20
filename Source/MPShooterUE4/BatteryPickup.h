// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "BatteryPickup.generated.h"

/**
 * 
 */
UCLASS()
class MPSHOOTERUE4_API ABatteryPickup : public APickup
{
	GENERATED_BODY()

public:
	//set default vals to instance of a class
	ABatteryPickup();

	//server side handling
	UFUNCTION(BlueprintAuthorityOnly, Category = "Pickup")
	void PickedUpBy(APawn* Pawn) override;
	

};
