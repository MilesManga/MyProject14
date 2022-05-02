// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyProject14HUD.generated.h"

UCLASS()
class AMyProject14HUD : public AHUD
{
	GENERATED_BODY()

public:
	AMyProject14HUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	virtual void BeginPlay() override;
private:
	/** Crosshair asset pointer */
	class UTexture2D * CrosshairTex;
	// Develops a userwidget class for the HUD.
	UPROPERTY(EditAnywhere, Category = "Health")
		TSubclassOf<class UUserWidget> HUDWidgetClass;
	// Displays the health widget in the HUD.
	UPROPERTY(EditAnywhere, Category = "Health")
		class UUserWidget * CurrentWidget;
};
