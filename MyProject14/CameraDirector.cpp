// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraDirector.h"
#include "Kismet/GameplayStatics.h"

// The different camera shots provide a new perspective every time it changes by giving the player a new view to see.

// Sets default values
ACameraDirector::ACameraDirector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACameraDirector::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACameraDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    // The amount of time for the camera transitations
    const float TimeBetweenCameraChanges = 2.0f;
    const float SmoothBlendTime = 0.75f;
    TimeToNextCameraChange -= DeltaTime;
    // Switches from one camera to another camera.
    if (TimeToNextCameraChange <= 0.0f)
    {
        TimeToNextCameraChange += TimeBetweenCameraChanges;

        //Find the actor that handles control for the local player.
        APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(this, 0);
        if (OurPlayerController)
        {
            // This gets us the view of the first camera positioned in the world.
            if ((OurPlayerController->GetViewTarget() != CameraOne) && (CameraOne != nullptr))
            {
                //Begins instantly to camera one.
                OurPlayerController->SetViewTarget(CameraOne);
            }
            // This gives us the view of the second camera, which the FirstPersonCharacter2 mesh.
            else if ((OurPlayerController->GetViewTarget() != CameraTwo) && (CameraTwo != nullptr))
            {
                //Blend smoothly to camera two.
                OurPlayerController->SetViewTargetWithBlend(CameraTwo, SmoothBlendTime);
            }
        }
    }
}

