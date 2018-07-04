// Fill out your copyright notice in the Description page of Project Settings.

#include "CollectorGameState.h"
#include "Net/UnrealNetwork.h"

ACollectorGameState::ACollectorGameState()
{
	//state default state when state is not currently known
	CurrentState = EBatteryPlayState::EUnknown;
}

void ACollectorGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACollectorGameState, PowerToWin);
	DOREPLIFETIME(ACollectorGameState, CurrentState);
	DOREPLIFETIME(ACollectorGameState, WinningPlayerName);
}

EBatteryPlayState ACollectorGameState::GetCurrentState()const
{
	return CurrentState;
}

void ACollectorGameState::SetCurrentState(EBatteryPlayState NewState)
{
	if (Role == ROLE_Authority)
	{
		CurrentState = NewState;
	}
}
void ACollectorGameState::OnRep_CurrentState()
{

}

FText ACollectorGameState::GetVictoryEnumAsString(uint8 EnumValue)
{
	const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EVictoryEnum"), true);
	if (!EnumPtr) return NSLOCTEXT("Invalid", "Invalid", "Invalid");

	return EnumPtr->GetDisplayNameText(EnumValue);
}
