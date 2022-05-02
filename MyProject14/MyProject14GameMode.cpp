// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "MyProject14GameMode.h"
#include "MyProject14HUD.h"
#include "MyProject14Character.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

AMyProject14GameMode::AMyProject14GameMode()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AMyProject14HUD::StaticClass();
}
// BeginPlay function occurs.
void AMyProject14GameMode::BeginPlay()
{
	Super::BeginPlay();

	SetCurrentState(EGamePlayState::EPlaying);

	MyCharacter = Cast<AMyProject14Character>(UGameplayStatics::GetPlayerPawn(this, 0));
}
// Creates a game over screen when the player's health reaches to zero.
void AMyProject14GameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GetWorld()->GetMapName();

	if (MyCharacter)
	{
		if (FMath::IsNearlyZero(MyCharacter->GetHealth(), 0.001f))
		{
			SetCurrentState(EGamePlayState::EGameOver);
		}
	}
}
// Gets the current state of the game mode.
EGamePlayState AMyProject14GameMode::GetCurrentState() const
{
	return CurrentState;
}
// Sets the current state of the game mode.
void AMyProject14GameMode::SetCurrentState(EGamePlayState NewState)
{
	CurrentState = NewState;
	HandleNewState(CurrentState);
}
// The function switches between the different game states leading to different results. 
void AMyProject14GameMode::HandleNewState(EGamePlayState NewState)
{
	switch (NewState)
	{
	case EGamePlayState::EPlaying:
	{
		// do nothing
	}
	break;
	// Unknown/default state
	case EGamePlayState::EGameOver:
	{
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	}
	break;
	// Unknown/default state
	default:
	case EGamePlayState::EUnknown:
	{
		// do nothing
	}
	break;
	}
}