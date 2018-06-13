// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "MPShooterUE4GameMode.h"
#include "MPShooterUE4Character.h"
#include "UObject/ConstructorHelpers.h"

AMPShooterUE4GameMode::AMPShooterUE4GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
