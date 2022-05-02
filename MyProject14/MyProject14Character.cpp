// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "MyProject14Character.h"
#include "MyProject14Projectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "TimerManager.h"
#include "Components/TimelineComponent.h"
#include "Engine.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AMyProject14Character

AMyProject14Character::AMyProject14Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;
}

void AMyProject14Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	// Health components that are connected to the HUD menu.
	FullHealth = 1000.0f;
	Health = FullHealth;
	HealthPercentage = 1.0f;
	bCanBeDamaged = true;
	// Magic components that are connected to the HUD menu.
	FullMagic = 100.0f;
	Magic = FullMagic;
	MagicPercentage = 1.0f;
	PreviousMagic = MagicPercentage;
	MagicValue = 0.0f;
	bCanUseMagic = true;
	// MagicCurve statement
	if (MagicCurve)
	{
		FOnTimelineFloat TimelineCallback;
		FOnTimelineEventStatic TimelineFinishedCallback;

		TimelineCallback.BindUFunction(this, FName("SetMagicValue"));
		TimelineFinishedCallback.BindUFunction(this, FName("SetMagicState"));

		MyTimeline = NewObject<UTimelineComponent>(this, FName("Magic Animation"));
		MyTimeline->AddInterpFloat(MagicCurve, TimelineCallback);
		MyTimeline->SetTimelineFinishedFunc(TimelineFinishedCallback);
		MyTimeline->RegisterComponent();
	}

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}
}

void AMyProject14Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MyTimeline != nullptr) MyTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, nullptr);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMyProject14Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind DisplayRaycast event
	PlayerInputComponent->BindAction("Raycast", IE_Pressed, this, &AMyProject14Character::DisplayRaycast);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AMyProject14Character::OnFire);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMyProject14Character::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyProject14Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyProject14Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMyProject14Character::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMyProject14Character::LookUpAtRate);
}

void AMyProject14Character::OnFire()
{
	// try and fire a projectile
	if (ProjectileClass != NULL && !FMath::IsNearlyZero(Magic, 0.001f) && bCanUseMagic)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			if (bUsingMotionControllers)
			{
				const FRotator SpawnRotation = VR_MuzzleLocation->GetComponentRotation();
				const FVector SpawnLocation = VR_MuzzleLocation->GetComponentLocation();
				World->SpawnActor<AMyProject14Projectile>(ProjectileClass, SpawnLocation, SpawnRotation);
			}
			else
			{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				World->SpawnActor<AMyProject14Projectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
			}
		}

		// try and play the sound if specified
		if (FireSound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		// try and play a firing animation if specified
		if (FireAnimation != NULL)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
			if (AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.f);
			}
		}
		// Timeline is stopped when the gun is overheated.
		if (MyTimeline != nullptr) MyTimeline->Stop();
		GetWorldTimerManager().ClearTimer(MagicTimerHandle);
		SetMagicChange(-20.0f);

	}
}

void AMyProject14Character::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

// DisplayRaycast functions displays a red line when a projectile hits a target. 
void AMyProject14Character::DisplayRaycast()
{
	FHitResult* Hitresult = new FHitResult();
	FVector StartTrace = FirstPersonCameraComponent->GetComponentLocation();
	FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();
	FVector EndTrace = ((ForwardVector * 3319.f) + StartTrace);
	FCollisionQueryParams* TraceParams = new FCollisionQueryParams();

	if (GetWorld()->LineTraceSingleByChannel(*Hitresult, StartTrace, EndTrace,
		ECC_Visibility, *TraceParams))
	{
		DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor(255, 0, 0), true);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("You hit: %s"),
			*Hitresult->Actor->GetName()));
	}
}

void AMyProject14Character::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AMyProject14Character::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void AMyProject14Character::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

// The MoveFoward function starts at 0. The valve increases based on the type of direction movement. It ulitizes a slope intercept function. 
void AMyProject14Character::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}
// The MoveRight function starts at 0. The valve increases based on the type of direction movement. It ulitizes a slope intercept function. 
void AMyProject14Character::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AMyProject14Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMyProject14Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AMyProject14Character::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AMyProject14Character::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AMyProject14Character::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AMyProject14Character::TouchUpdate);
		return true;
	}
	
	return false;
}

// Gets the player's health percentage
float AMyProject14Character::GetHealth()
{
	return HealthPercentage;
}

// Gets the player's magic percentage
float AMyProject14Character::GetMagic()
{
	return MagicPercentage;
}
// Gets Health text
FText AMyProject14Character::GetHealthIntText()
{
	int32 HP = FMath::RoundHalfFromZero(HealthPercentage * 100);
	FString HPS = FString::FromInt(HP);
	FString HealthHUD = HPS + FString(TEXT("%"));
	FText HPText = FText::FromString(HealthHUD);
	return HPText;
}
// Gets Magic text
FText AMyProject14Character::GetMagicIntText()
{
	int32 MP = FMath::RoundHalfFromZero(MagicPercentage * FullMagic);
	FString MPS = FString::FromInt(MP);
	FString FullMPS = FString::FromInt(FullMagic);
	FString MagicHUD = MPS + FString(TEXT("/")) + FullMPS;
	FText MPText = FText::FromString(MagicHUD);
	return MPText;
}

// Gets the SetDamageState function so the character can be damaged.
void AMyProject14Character::SetDamageState()
{
	bCanBeDamaged = true;
}
// The DamagerTimer is set at 20 frames and it takes away a chunk of health from the character.
void AMyProject14Character::DamageTimer()
{
	GetWorldTimerManager().SetTimer(MemberTimerHandle, this, &AMyProject14Character::SetDamageState, 2.0f, false);
}
// SetMagicValue function
void AMyProject14Character::SetMagicValue()
{
	TimelineValue = MyTimeline->GetPlaybackPosition();
	CurveFloatValue = PreviousMagic + MagicValue * MagicCurve->GetFloatValue(TimelineValue);
	Magic = FMath::Clamp(CurveFloatValue * FullHealth, 0.0f, FullMagic);
	MagicPercentage = FMath::Clamp(CurveFloatValue, 0.0f, 1.0f);
}
// SetsMagicState function
void AMyProject14Character::SetMagicState()
{
	bCanUseMagic = true;
	MagicValue = 0.0;
	if (GunDefaultMaterial)
	{
		FP_Gun->SetMaterial(0, GunDefaultMaterial);
	}
}
// Plays a flash when the character is taking damage.
bool AMyProject14Character::PlayFlash()
{
	if (redFlash)
	{
		redFlash = false;
		return true;
	}

	return false;
}
// TakeDamage function
float AMyProject14Character::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	bCanBeDamaged = false;
	redFlash = true;
	UpdateHealth(-DamageAmount);
	DamageTimer();
	return DamageAmount;
}

// UpdateHealth function
void AMyProject14Character::UpdateHealth(float HealthChange)
{
	Health += HealthChange;
	Health = FMath::Clamp(Health, 0.0f, FullHealth);
	PreviousHealth = HealthPercentage;
	HealthPercentage = Health / FullHealth;
}
// UpdateMagic function
void AMyProject14Character::UpdateMagic(float MagicChange)
{
	PreviousMagic = MagicPercentage;
	MagicPercentage = Magic / FullMagic;
	MagicValue = 1.0f;
	if (MyTimeline != nullptr) MyTimeline->PlayFromStart();
}
// SetsMagicChange function
void AMyProject14Character::SetMagicChange(float MagicChange)
{
	bCanUseMagic = false;
	PreviousMagic = MagicPercentage;
	MagicValue = (MagicChange / FullMagic);

	if (GunOverheatMaterial) FP_Gun->SetMaterial(0, GunOverheatMaterial);

	if (MyTimeline != nullptr) MyTimeline->PlayFromStart();
	}

	