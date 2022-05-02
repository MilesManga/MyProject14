// Fill out your copyright notice in the Description page of Project Settings.


#include "HealingKit.h"

// Sets default values
AHealingKit::AHealingKit()
{
	OnActorBeginOverlap.AddDynamic(this, &AHealingKit::OnOverlap);

}
// An OnOverlap function occurs for the healing kit function.
void AHealingKit::OnOverlap(AActor* MyOverlappedActor, AActor* OtherActor)
{
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		MyCharacter = Cast<AMyProject14Character>(OtherActor);

		if (MyCharacter && MyCharacter->GetHealth() < 1.0f)
		{
			// Updates the character's health and then its destroyed.
			MyCharacter->UpdateHealth(100.0f);
			Destroy();
		}
	}
}

