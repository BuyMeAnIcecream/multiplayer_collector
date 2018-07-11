// Fill out your copyright notice in the Description page of Project Settings.

#include "Banana.h"




ABanana::ABanana()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;
	GetStaticMeshComponent()->bGenerateOverlapEvents = true;

	//sync server and client movement
	bReplicateMovement = true;
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	GetStaticMeshComponent()->SetSimulatePhysics(true);
	
	if (Role == ROLE_Authority) {
		bIsActive = true;
	}

	//add movement and rotation components
	MovComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	RotMovComp = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));

}


void ABanana::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABanana, bIsActive);
}

bool ABanana::IsActive()
{
	return bIsActive;
}

void ABanana::SetActive(bool NewActive) {
	if (Role == ROLE_Authority)
	{
		bIsActive = NewActive;
	}
}

void ABanana::OnRep_IsActive()
{

}

