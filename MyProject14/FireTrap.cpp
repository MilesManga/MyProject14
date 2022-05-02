// Fill out your copyright notice in the Description page of Project Settings.


#include "FireTrap.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

// Sets default values
AFireTrap::AFireTrap()
{
    // Creates the BoxComponent for the FireTrap
    MyBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("My Box Component"));
    MyBoxComponent->InitBoxExtent(FVector(50.0f, 50.0f, 50.0f));
    RootComponent = MyBoxComponent;
    // Creates the Fire for the FireTrap
    Fire = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("My Fire"));
    Fire->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    Fire->SetupAttachment(RootComponent);
    // The Overlapping components for the FireTrap
    MyBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AFireTrap::OnOverlapBegin);
    MyBoxComponent->OnComponentEndOverlap.AddDynamic(this, &AFireTrap::OnOverlapEnd);

    bCanApplyDamage = false;


}
// The OverlapBegin function for the FireTrap with a TimerHandle
void AFireTrap::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
    {
        bCanApplyDamage = true;
        MyCharacter = Cast<AActor>(OtherActor);
        MyHit = SweepResult;
        GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AFireTrap::ApplyFireDamage, 2.2f, true, 0.0f);
    }
}
// The OverlapEnd function for the FireTrap with a TimerHandle function
void AFireTrap::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    bCanApplyDamage = false;
    GetWorldTimerManager().ClearTimer(FireTimerHandle);
}
// Character gets hurt from touching the FireTrap
void AFireTrap::ApplyFireDamage()
{
    if (bCanApplyDamage)
    {
        UGameplayStatics::ApplyPointDamage(MyCharacter, 200.0f, GetActorLocation(), MyHit, nullptr, this, FireDamageType);
    }
}

