// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Banana.generated.h"

/**
 * 
 */
UCLASS()
class MPSHOOTERUE4_API ABanana : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	ABanana();

	UFUNCTION(BlueprintPure, Category = "Banana")
	bool IsActive();

//	UFUNCTION(BlueprintCallable, Category = "Pickup")
//	void SetActive(bool newPickupState);

	//Required network scuffolding
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintAuthorityOnly, Category = "Banana")
	virtual void KnockOut(APawn* Pawn);
	void KnockOut_Implementation();

protected:
	//can be used
	UPROPERTY(ReplicatedUsing = OnRep_IsActive)
	bool bIsActive;
};
