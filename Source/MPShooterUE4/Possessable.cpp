// Fill out your copyright notice in the Description page of Project Settings.

#include "Possessable.h"
#include "Components/TimelineComponent.h"
#include <string>

// Sets default values
APossessable::APossessable()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComponent;

	if (Role == ROLE_Authority) {
		bContainsPlayer = false;
	}

	//CAM CONTROLS
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//Load Cam Trans Curve
	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve(TEXT("/Game/Timelines/fCamTransCurve"));
	check(Curve.Succeeded());

	CamTransCurve = Curve.Object;
}

void APossessable::BeginPlay()
{
	//Timeline callback 
	FOnTimelineFloat onTimelineCallback;
	FOnTimelineEventStatic onTimelineFinishedCallback;

	Super::BeginPlay();
	
	if (CamTransCurve != NULL)
	{
		MyTimeline = NewObject<UTimelineComponent>(this, FName("TimelineAnimation"));
		MyTimeline->CreationMethod = EComponentCreationMethod::UserConstructionScript; // Indicate it comes from a blueprint so it gets cleared when we rerun construction scripts
		this->BlueprintCreatedComponents.Add(MyTimeline); // Add to array so it gets saved
		MyTimeline->SetNetAddressable();	// This component has a stable name that can be referenced for replication

		MyTimeline->SetPropertySetObject(this); // Set which object the timeline should drive properties on
		MyTimeline->SetDirectionPropertyName(FName("TimelineDirection"));
//		CamTransCurve
		MyTimeline->SetLooping(false);
		MyTimeline->SetTimelineLength(0.7f);
		MyTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);

		MyTimeline->SetPlaybackPosition(0.0f, false);

		//Add the float curve to the timeline and connect it to your timelines's interpolation function
		onTimelineCallback.BindUFunction(this, FName{ TEXT("TimelineCallback") });
		onTimelineFinishedCallback.BindUFunction(this, FName{ TEXT("TimelineFinishedCallback") });
		MyTimeline->AddInterpFloat(CamTransCurve, onTimelineCallback);
		MyTimeline->SetTimelineFinishedFunc(onTimelineFinishedCallback);

		MyTimeline->RegisterComponent();
	}

	//saving the position of transform setup by designer as desired one
	CamDesiredTrans = FollowCamera->GetComponentTransform();
	CharacterCamTrans = FTransform(FVector(0, 0, 0));
}


void APossessable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MyTimeline != NULL)
	{
		MyTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, NULL);
	}
}

void APossessable::TimelineCallback(float interpolatedVal)
{
	// This function is called for every tick in the timeline.
	float CamTransCurveVal = CamTransCurve->GetFloatValue(MyTimeline->GetPlaybackPosition());
	// TODO cam init loc +  (cam init loc + (cam destin - cam init)) * curve val
	FollowCamera->SetWorldLocation(CharacterCamTrans.GetLocation() + (CamDesiredTrans.GetLocation() - CharacterCamTrans.GetLocation()) * CamTransCurveVal);
	FollowCamera->SetWorldRotation(CharacterCamTrans.GetRotation() + (CamDesiredTrans.GetRotation() - CharacterCamTrans.GetRotation()) * CamTransCurveVal);
/*	if (GEngine) {
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("timeline val: " + FString::SanitizeFloat(CamTransCurveVal)));
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Yellow, TEXT("MyCharacter's Location is " +  FollowCamera->GetComponentLocation().ToString()));
	}
*/
}

void APossessable::TimelineFinishedCallback()
{
	// This function is called when the timeline finishes playing.
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	EnableInput(PlayerController);
}

void APossessable::PlayTimeline()
{
	if (MyTimeline != NULL)
	{
		MyTimeline->PlayFromStart();
	}
}

void APossessable ::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APossessable, bContainsPlayer);
}

void APossessable::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APossessable::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool APossessable::ContainsPlayer()
{
	return bContainsPlayer;
}

void APossessable::SetContainsPlayer(bool NewContains) {
	if (Role == ROLE_Authority)
	{
		bContainsPlayer = NewContains;
	}
}

void APossessable::HidePlayer(APawn * Pawn, FTransform CharCamTrans)
{
	//Set contains player true
	if (Role == ROLE_Authority)
	{
		//Allow ragdoll to penetrate
		//		StaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
		SetContainsPlayer(true);
		//		if (GEngine) {
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Pawn->GetName());
		//}
				CharacterCamTrans = CharCamTrans;
				StartCameraMovement();
		
	}
	else
	{
		//CamLastTrans);
	}
	//Play timeline

	//Postpone
}

void APossessable::ClientHidePlayer_Implementation(APawn * Pawn)
{
}

void APossessable::OnRep_ContainsPlayer()
{

}

void APossessable::StartCameraMovement()//FTransform CamLastTrans)
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	DisableInput(PlayerController);
	//GetController()->inputallowed
	//TODO disable input
	//TODO set camera to CamLocation and play timeline
	PlayTimeline();
}



//////////////////////////////////////////////////////////////////////////
// Input
// Called to bind functionality to input
void APossessable::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &APossessable::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APossessable::LookUpAtRate);

	//handle hiding
//TODO	PlayerInputComponent->BindAction("Hide", IE_Pressed, this, &AMPShooterUE4Character::Hide);
}
