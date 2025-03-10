// Copyright Epic Games, Inc. All Rights Reserved.

#include "DegreeProjectGameMode.h"
#include "DegreeProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

ADegreeProjectGameMode::ADegreeProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_Knight"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
