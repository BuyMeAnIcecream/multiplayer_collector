// Fill out your copyright notice in the Description page of Project Settings.

#include "HidableMesh.h"
#include "Runtime/Engine/Public/EngineGlobals.h"




AHidableMesh::AHidableMesh()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = false;
	GetStaticMeshComponent()->bGenerateOverlapEvents = true;

	//Set up collision to block characters by default

	if (Role == ROLE_Authority) {
		bContainsPlayer = false;
	}
}


void AHidableMesh::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHidableMesh, bContainsPlayer);
}

bool AHidableMesh::ContainsPlayer()
{
	return bContainsPlayer;
}

void AHidableMesh::SetContainsPlayer(bool NewContains) {
	if (Role == ROLE_Authority)
	{
		bContainsPlayer = NewContains;
	}
}

void AHidableMesh::HidePlayer(APawn * Pawn)
{
	//Set contains player true
	if (Role == ROLE_Authority)
	{
		SetContainsPlayer(true);
/*		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Pawn->GetName());
		}
*/
	}
	
	//Play timeline

	//Postpone
}

void AHidableMesh::ClientHidePlayer_Implementation(APawn * Pawn)
{
}

void AHidableMesh::OnRep_ContainsPlayer()
{

}

