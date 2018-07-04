// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MPShooterUE4Character.generated.h"


UCLASS(config=Game)
class AMPShooterUE4Character : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** CollectionSphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Battery, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollectionSphere;
public:
	AMPShooterUE4Character();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UFUNCTION(BlueprintPure, Category = "Power")
	float GetInitialPower();

	UFUNCTION(BlueprintPure, Category = "Power")
	float GetCurrentPower();

	/**
	*Updates power lvl
	* @param DeltaPower is the amount that is added to CurrentPower
	*/
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Power")
	void UpdatePower(float DeltaPower);

	//shutdown pawn and ragdoll it on all clients
	UFUNCTION(NetMulticast, Reliable)
	void OnPlayerDeath();
protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	//called on key collect pressed
	UFUNCTION(BlueprintCallable, Category = "Pickups")
	void CollectPickups();

	//process collection on serv
	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCollectPickups();

	//starting pwrlevel
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float InitialPower;

	//spped at 0 power
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float BaseSpeed;

	//multiplier fpr controlling speed depending on power level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float SpeedFactor;

	//update character visuals based on current power level
	UFUNCTION(BlueprintImplementableEvent, Category = "Power")
	void PowerChangeEffect();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns CollectionSphere subobject **/
	FORCEINLINE class USphereComponent* GetCollectionSphere() const { return CollectionSphere; }
private:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Battery", Meta = (AllowPrivateAccess = "true"))
	float CollectionSphereRadius;

	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentPower, VisibleAnywhere, Category = "Power")
	float CurrentPower;

	// Power level is updated on clients
	UFUNCTION()
	void OnRep_CurrentPower();
};

