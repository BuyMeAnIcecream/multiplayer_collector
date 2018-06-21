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

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//server side handling
	UFUNCTION(BlueprintAuthorityOnly, Category = "Pickup")
	void PickedUpBy(APawn* Pawn) override;
	
	float GetPower();

protected:
	//power
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float BatteryPower;
};
