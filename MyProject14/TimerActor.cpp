// Fill out your copyright notice in the Description page of Project Settings.


#include "TimerActor.h"
#include "Engine.h"
// Sets default values
ATimerActor::ATimerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATimerActor::BeginPlay()
{
	Super::BeginPlay();

	// connect timer function to actor. After 5 seconds run RepeatingFunction every 2 seconds 
	GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &ATimerActor::RepeatingFunction, 2.0f, true, 5.0f);
	
}

// Called every frame
void ATimerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
// The repeating function displays a red text stating "Get Ready to Rumble!" This appears every five seconds.
void ATimerActor::RepeatingFunction()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Get Ready to Rumble!"));
}

