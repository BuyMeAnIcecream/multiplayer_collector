// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Engine/StaticMeshActor.h"
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
	//set default vals to instance of a class
	ABanana();
	
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "Banana")
	bool IsActive();

	UFUNCTION(BlueprintCallable, Category = "Banana")
	void SetActive(bool NewActive);

	
protected:
	//Rotating movement component
	UPROPERTY()
	URotatingMovementComponent* RotMovComp;

	//Movement component
	UPROPERTY()
	UProjectileMovementComponent* MovComp;
	
	//can be used
	UPROPERTY(ReplicatedUsing = OnRep_IsActive)
	bool bIsActive;

	UFUNCTION()
	virtual void OnRep_IsActive();
};
