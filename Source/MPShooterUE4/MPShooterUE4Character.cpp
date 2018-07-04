// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "MPShooterUE4Character.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "BatteryPickup.h"
//////////////////////////////////////////////////////////////////////////
// AMPShooterUE4Character

AMPShooterUE4Character::AMPShooterUE4Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//base value for sphere radius
	CollectionSphereRadius = 200.0f;

    // Create a Collection Sphere
	CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	CollectionSphere->AttachToComponent(RootComponent,FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	CollectionSphere->SetSphereRadius(CollectionSphereRadius);
	

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	//base vals for char power
	InitialPower = 2000.0f;
	CurrentPower = InitialPower;

	//base values for controlling movement speed
	BaseSpeed = 10.0f; //epic's default
	SpeedFactor = 0.75f;
}

void AMPShooterUE4Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMPShooterUE4Character, CollectionSphereRadius);
	DOREPLIFETIME(AMPShooterUE4Character, InitialPower);
	DOREPLIFETIME(AMPShooterUE4Character, CurrentPower);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMPShooterUE4Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
check(PlayerInputComponent);
PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

PlayerInputComponent->BindAxis("MoveForward", this, &AMPShooterUE4Character::MoveForward);
PlayerInputComponent->BindAxis("MoveRight", this, &AMPShooterUE4Character::MoveRight);

// We have 2 versions of the rotation bindings to handle different kinds of devices differently
// "turn" handles devices that provide an absolute delta, such as a mouse.
// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
PlayerInputComponent->BindAxis("TurnRate", this, &AMPShooterUE4Character::TurnAtRate);
PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
PlayerInputComponent->BindAxis("LookUpRate", this, &AMPShooterUE4Character::LookUpAtRate);

// handle touch devices
PlayerInputComponent->BindTouch(IE_Pressed, this, &AMPShooterUE4Character::TouchStarted);
PlayerInputComponent->BindTouch(IE_Released, this, &AMPShooterUE4Character::TouchStopped);

// VR headset functionality
PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMPShooterUE4Character::OnResetVR);

//handle pickups
PlayerInputComponent->BindAction("CollectPickups", IE_Pressed, this, &AMPShooterUE4Character::CollectPickups);
}


float AMPShooterUE4Character::GetInitialPower()
{
	return InitialPower;
}

float AMPShooterUE4Character::GetCurrentPower()
{
	return CurrentPower;
}

void AMPShooterUE4Character::UpdatePower(float DeltaPower)
{
	if (Role == ROLE_Authority)
	{
		CurrentPower += DeltaPower;
		//set movement speed based on power level
		GetCharacterMovement()->MaxWalkSpeed = BaseSpeed + SpeedFactor * CurrentPower;

		//fake the rep notify (listen server doesn't get the repnotify automatically
		OnRep_CurrentPower();
	}
}

void AMPShooterUE4Character::OnPlayerDeath_Implementation()
{
	//disconnect controller from Pawn
	DetachFromControllerPendingDestroy();
	if (GetMesh())
	{
		static FName CollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetCollisionProfileName(CollisionProfileName);
	}

	SetActorEnableCollision(true);

	//ragdoll (init physics)
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;

	//disable movment
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);

	//disable collisions of the capsule
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void AMPShooterUE4Character::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMPShooterUE4Character::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AMPShooterUE4Character::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AMPShooterUE4Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMPShooterUE4Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMPShooterUE4Character::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMPShooterUE4Character::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AMPShooterUE4Character::CollectPickups()
{
	//ask server to collect dat
	ServerCollectPickups();
}

void AMPShooterUE4Character::OnRep_CurrentPower()
{
	PowerChangeEffect();
}

bool AMPShooterUE4Character::ServerCollectPickups_Validate()
{
	return true;
}

void AMPShooterUE4Character::ServerCollectPickups_Implementation()
{
	if (Role == ROLE_Authority)
	{
		//track total power found in batteries
		float TotalPower = 0.0f;
		//get all overlapping actors
		TArray<AActor*> CollectedActors;
		CollectionSphere->GetOverlappingActors(CollectedActors);
		//check if they are pickups
		for (int i = 0; i < CollectedActors.Num(); i++)
		{
			APickup* const TestPickup = Cast<APickup>(CollectedActors[i]);
			if (TestPickup != NULL && !TestPickup->IsPendingKill() && TestPickup->IsActive()) 
			{
				//add power if battery was found
				if (ABatteryPickup* const TestBattery = Cast<ABatteryPickup>(TestPickup))
				{
					TotalPower += TestBattery->GetPower();
				}
				//collect and deactivat
				TestPickup->PickedUpBy(this);
				TestPickup->SetActive(false);
			}	
		}

		//change the character power based on what we pickedup
		if (!FMath::IsNearlyZero(TotalPower, 0.01f))
		{
			UpdatePower(TotalPower);
		}
	}
}