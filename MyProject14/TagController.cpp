// Fill out your copyright notice in the Description page of Project Settings.


#include "TagController.h"
#include "Engine/World.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// Tells the AI to go to a random waypoint on the map from the ATargetPoint class
void ATagController::BeginPlay()
{
    Super::BeginPlay();

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), Waypoints);

    GoToRandomWaypoint();
}
// Calucates the number of waypoints on the map and subtracts by 1.
ATargetPoint* ATagController::GetRandomWaypoint()
{
    auto index = FMath::RandRange(0, Waypoints.Num() - 1);
    return Cast<ATargetPoint>(Waypoints[index]);
}
// The function tells the AI to go any random waypoint on the map.
void ATagController::GoToRandomWaypoint()
{
    MoveToActor(GetRandomWaypoint());
}
// The function uses the GetWorldTimerMananger for the OnMoveCompleted function to connect to the GoToRandomWaypoint function.
void ATagController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);

    GetWorldTimerManager().SetTimer(TimerHandle, this, &ATagController::GoToRandomWaypoint, 1.0f, false);

    GoToRandomWaypoint();
}

