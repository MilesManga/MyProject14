// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "FireTrap.generated.h"

UCLASS()
class MYPROJECT14_API AFireTrap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFireTrap();
public:	
	// Creates a particle effect with the fire.
	UPROPERTY(EditAnywhere)
		UParticleSystemComponent* Fire;
	// A box component is created for the fire.
	UPROPERTY(EditAnywhere)
		UBoxComponent* MyBoxComponent;
	// The TSubclass classifies the damage type as fire. 
	UPROPERTY(EditAnywhere)
		TSubclassOf<UDamageType> FireDamageType;

	UPROPERTY(EditAnywhere)
		AActor* MyCharacter;
	// A Hitresult function is created for the fire damage type
	UPROPERTY(EditAnywhere)
		FHitResult MyHit;
	// CanApplyDamage function
	bool bCanApplyDamage;
	FTimerHandle FireTimerHandle;

	// declare overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// declare overlap end function
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	// The function applies fire damage to the character. 
	UFUNCTION()
		void ApplyFireDamage();

};
