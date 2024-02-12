// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExperimentalFPSGameMode.h"
#include "ExperimentalFPSCharacter.h"
#include "UObject/ConstructorHelpers.h"

AExperimentalFPSGameMode::AExperimentalFPSGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
