// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine.h"
#include "MyProject14Character.generated.h"


class UInputComponent;
class UTimelineComponent;

UCLASS(config=Game)
class AMyProject14Character : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* VR_Gun;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* VR_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* L_MotionController;

public:
	AMyProject14Character();

protected:
	virtual void BeginPlay();

	void Tick(float DeltaTime);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser);

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AMyProject14Projectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint32 bUsingMotionControllers : 1;

	// FullHealth function
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float FullHealth;
	// Health function
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float Health;
	// Displays the player's health percentage.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float HealthPercentage;
	// PreviousHealth function
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float PreviousHealth;
	// FullMagic function
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
		float FullMagic;
	// Magic function
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
		float Magic;
	// Displays the player's magic percentage.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
		float MagicPercentage;
	// PreviousMagic function
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
		float PreviousMagic;
	// MagicValue function
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
		float MagicValue;
	// A red flash appears when the player is being hit.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float redFlash;
	// Creates a MagicCurve function.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magic")
		UCurveFloat* MagicCurve;

	float CurveFloatValue;
	float TimelineValue;
	bool bCanUseMagic;

	UTimelineComponent* MyTimeline;
	FTimerHandle MemberTimerHandle;
	FTimerHandle MagicTimerHandle;

	/** Get Health */
	UFUNCTION(BlueprintPure, Category = "Health")
		float GetHealth();

	/** Get Magic */
	UFUNCTION(BlueprintPure, Category = "Magic")
		float GetMagic();

	/** Get Health Text */
	UFUNCTION(BlueprintPure, Category = "Health")
		FText GetHealthIntText();

	/** Get Magic Text */
	UFUNCTION(BlueprintPure, Category = "Magic")
		FText GetMagicIntText();
	


	/** Damage Timer */
	UFUNCTION()
		void DamageTimer();

	/** Set Damage State */
	UFUNCTION()
		void SetDamageState();

	/** Set Magic Value */
	UFUNCTION()
		void SetMagicValue();

	/** Set Magic State */
	UFUNCTION()
		void SetMagicState();

	/** Set Magic Change State */
	UFUNCTION()
		void SetMagicChange(float MagicChange);

	/** Set UpdateMagic State */
	UFUNCTION()
		void UpdateMagic(float MagicChange);

	/** Play Flash */
	UFUNCTION(BlueprintPure, Category = "Health")
		bool PlayFlash();
    /** GunDefault Material  */
	UPROPERTY(EditAnywhere, Category = "Magic")
		class UMaterialInterface* GunDefaultMaterial;
	/** GunOverheatMaterial function */
	UPROPERTY(EditAnywhere, Category = "Magic")
		class UMaterialInterface* GunOverheatMaterial;
	
	// Power function
	UFUNCTION(BlueprintCallable, Category = "Power")
		void UpdateHealth(float HealthChange);

protected:
	
	/** Fires a projectile. */
	void OnFire();

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	private:
		void DisplayRaycast();
};

