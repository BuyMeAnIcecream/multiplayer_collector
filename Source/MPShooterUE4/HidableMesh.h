// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Pawn.h"
#include "HidableMesh.generated.h"

/**
 * 
 */
UCLASS()
class MPSHOOTERUE4_API AHidableMesh : public APawn
{
	GENERATED_BODY()
	
public:
	
	//set default vals to instance of a class
	AHidableMesh();
	
	//Adding mesh component
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Mesh")
	UStaticMeshComponent* StaticMeshComponent;
//	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "HidableMesh")
	bool ContainsPlayer();

	UFUNCTION(BlueprintCallable, Category = "HidableMesh")
	void SetContainsPlayer(bool NewContains);

	//server side handling of hidding a character
	UFUNCTION(BlueprintAuthorityOnly, Category = "HidableMesh")
	virtual void HidePlayer(APawn* Pawn);
	//ClientSide
	void ClientHidePlayer_Implementation(APawn * Pawn);
	
protected:
	//can be used
	UPROPERTY(ReplicatedUsing = OnRep_ContainsPlayer)
	bool bContainsPlayer;

	UFUNCTION()
	virtual void OnRep_ContainsPlayer();
	

};
