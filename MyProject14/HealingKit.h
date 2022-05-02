// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyProject14Character.h"
#include "HealingKit.generated.h"

UCLASS()
class MYPROJECT14_API AHealingKit : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHealingKit();

public:	
	// Creates an overlapping function for the healing kit
	UFUNCTION()
		void OnOverlap(AActor* MyOverlappedActor, AActor* OtherActor);

	UPROPERTY(EditAnywhere)
		AMyProject14Character* MyCharacter;
};
