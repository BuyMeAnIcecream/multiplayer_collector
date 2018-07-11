// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CollectorGameState.generated.h"

UENUM(BlueprintType)
enum EBatteryPlayState
{
	EPlaying,
	EGameOver,
	EWon,
	EUnknown
};
/**
 * 
 */

UCLASS()
class MPSHOOTERUE4_API ACollectorGameState : public AGameStateBase
{
	GENERATED_BODY()


public:
	ACollectorGameState();

	//track power level required to win the game
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Power")
	float PowerToWin;

	//required network scaffolding
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "Power")
	EBatteryPlayState GetCurrentState() const;

	//Transition the game to a new playstate. Useable on the server only
	void SetCurrentState(EBatteryPlayState NewState);

	//Rep Notify fired on clients to allow for client-side changes in gameplay state
	UFUNCTION()
		void OnRep_CurrentState();

	//The name of winner
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Power")
	FString WinningPlayerName;
private:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentState)
	TEnumAsByte<enum EBatteryPlayState> CurrentState;

//	FText GetVictoryEnumAsString(uint8 EnumValue);
};
