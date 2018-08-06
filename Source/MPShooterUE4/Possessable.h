// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "Possessable.generated.h"


UCLASS()
class MPSHOOTERUE4_API APossessable : public APawn
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	//PAWN PROPS
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Mesh")
	UStaticMeshComponent* StaticMeshComponent;
	// Sets default values for this pawn's properties
	APossessable();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "HidableMesh")
	bool ContainsPlayer();

	UFUNCTION(BlueprintCallable, Category = "HidableMesh")
	void SetContainsPlayer(bool NewContains);

	//server side handling of hidding a character
	UFUNCTION(BlueprintAuthorityOnly, Category = "HidableMesh")
	virtual void HidePlayer(APawn* Pawn, FTransform CharCamTrans);
	//ClientSide
	void ClientHidePlayer_Implementation(APawn * Pawn);

	
protected:
	//TIMELINE FUNCTIONALITY
	UPROPERTY()
	UTimelineComponent* MyTimeline;

	//OnPossess camera transition curve
	UPROPERTY()
	UCurveFloat* CamTransCurve;

	UFUNCTION()
	void TimelineCallback(float val);

	UFUNCTION()
	void TimelineFinishedCallback();

	void PlayTimeline();

	UPROPERTY()
	TEnumAsByte<ETimelineDirection::Type> TimelineDirection;

	//can be used
	UPROPERTY(ReplicatedUsing = OnRep_ContainsPlayer)
	bool bContainsPlayer;

	UFUNCTION()
	virtual void OnRep_ContainsPlayer();

//	UFUNCTION(BlueprintImplementableEvent)
	void StartCameraMovement();//FTransform CamLastTrans);

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


	//camera desired transform
	FTransform CamDesiredTrans;
	
	//camera transform that will be set immidiately on possess
	FTransform CharacterCamTrans;

public:	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	
};
