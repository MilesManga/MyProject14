// Fill out your copyright notice in the Description page of Project Settings.


#include "LightSwitchCodeOnly.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ALightSwitchCodeOnly::ALightSwitchCodeOnly()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Creates the intensity of the light source.
	LightIntensity = 3000.0f;
	// PointLight is developed
	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Point Light"));
	PointLight->Intensity = LightIntensity;
	PointLight->bVisible = true;
	RootComponent = PointLight;
	// Creates a light sphere component. 
	LightSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Light Sphere Component"));
	LightSphere->InitSphereRadius(300.0f);
	LightSphere->SetCollisionProfileName(TEXT("Trigger"));
	LightSphere->SetupAttachment(RootComponent);

	LightSphere->OnComponentBeginOverlap.AddDynamic(this, &ALightSwitchCodeOnly::OnOverlapBegin);
	LightSphere->OnComponentEndOverlap.AddDynamic(this, &ALightSwitchCodeOnly::OnOverlapEnd);



}

// Called when the game starts or when spawned
void ALightSwitchCodeOnly::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALightSwitchCodeOnly::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
// The light switch is turned on when the character is not nearby. 
void ALightSwitchCodeOnly::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		ToggleLight();
	}
}
// The light switch is turned off when the character is standing on top of it.
void ALightSwitchCodeOnly::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		ToggleLight();
	}
}
// Displays the point light source. 
void ALightSwitchCodeOnly::ToggleLight()
{
	PointLight->ToggleVisibility();
}

