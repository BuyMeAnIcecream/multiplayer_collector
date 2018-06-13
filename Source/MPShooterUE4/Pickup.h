// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Pickup.generated.h"

/**
 * 
 */
UCLASS()
class MPSHOOTERUE4_API APickup : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	APickup();
	UFUNCTION(BlueprintPure, Category = "Pickup")
	bool IsActive();

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void SetActive(bool newPickupState);

	//Required network scuffolding
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void WasCollected();
	virtual void WasCollected_Implementation();

protected:
	//can be used
	UPROPERTY(ReplicatedUsing = OnRep_IsActive)
	bool bIsActive;

	UFUNCTION()
	virtual void OnRep_IsActive();
	
};
