// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyProject14GameMode.generated.h"

// The class showscases several results when playing the level.
UENUM()
enum class EGamePlayState
{
	EPlaying,
	EGameOver,
	EUnknown
};

UCLASS(minimalapi)
class AMyProject14GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyProject14GameMode();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// Connects to the MyProject14Character files.
	class AMyProject14Character* MyCharacter;

	/** Returns the current playing state */
	UFUNCTION(BlueprintPure, Category = "Health")
		EGamePlayState GetCurrentState() const;

	/** Sets a new playing state */
	void SetCurrentState(EGamePlayState NewState);

private:
	/**Keeps track of the current playing state */
	EGamePlayState CurrentState;

	/**Handle any function calls that rely upon changing the playing state of our game */
	void HandleNewState(EGamePlayState NewState);
};



