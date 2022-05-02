// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Runtime/Engine/Classes/Engine/TargetPoint.h"
#include "TagController.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT14_API ATagController : public AAIController
{
	GENERATED_BODY()
// BeginPlay function
public:
    void BeginPlay() override;
// OnMoveCompleted function. This allows the AI to move in a complete loop around the map from the waypoints.
public:
    void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

private:
    // Connects to the waypoints on the map.
    UPROPERTY()
        TArray<AActor*> Waypoints;
    // Gets a random waypoint from the ATargetPoint function.
    UFUNCTION()
        ATargetPoint* GetRandomWaypoint();
    // Tells the AI to go a random waypoint on the map.
    UFUNCTION()
        void GoToRandomWaypoint();
private:
    FTimerHandle TimerHandle;
};
