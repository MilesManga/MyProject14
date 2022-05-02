// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OnComponentHit.generated.h"

UCLASS()
class MYPROJECT14_API AOnComponentHit : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOnComponentHit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Creates a box component
	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* MyComp;
	// Creates a component hit function inside the box.
	UFUNCTION()
		void OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
