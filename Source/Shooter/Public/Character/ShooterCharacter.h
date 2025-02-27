// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "SkeletalMergingLibrary.h"
#include "ShooterCharacter.generated.h"

class AWeapon;
class UCameraComponent;
class UCharacterDataAsset;
class UCharacterCombatComponent;
class UCharacterInventoryComponent;
class UShooterCharacterMovementComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AShooterCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	virtual void RecalculateBaseEyeHeight() override;

protected:
	virtual void BeginPlay() override;

public:
	void Init();

	UFUNCTION(BlueprintCallable, Category = Character)
	virtual bool CanProne() const;

	UFUNCTION(BlueprintCallable, Category = Character, meta = (HidePin = "bClientSimulation"))
	virtual void Prone(bool bClientSimulation = false);

	UFUNCTION(BlueprintCallable, Category = Character, meta = (HidePin = "bClientSimulation"))
	virtual void UnProne(bool bClientSimulation = false);

	UFUNCTION(BlueprintCallable, Category = Character)
	virtual bool CanSlow() const;

	UFUNCTION(BlueprintCallable, Category = Character, meta = (HidePin = "bClientSimulation"))
	virtual void Slow();

	UFUNCTION(BlueprintCallable, Category = Character, meta = (HidePin = "bClientSimulation"))
	virtual void UnSlow();

	UFUNCTION(BlueprintCallable, Category = Character)
	virtual bool CanSprint() const;

	UFUNCTION(BlueprintCallable, Category = Character, meta = (HidePin = "bClientSimulation"))
	virtual void Sprint();

	UFUNCTION(BlueprintCallable, Category = Character, meta = (HidePin = "bClientSimulation"))
	virtual void UnSprint();

	float GetAO_Pitch(float CurrentPitch, float DeltaTime) const;
	float GetAO_Yaw(float CurrentYaw, float DeltaTime) const;
	FVector GetCurrentAcceleration() const;
	
	virtual void OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);
	virtual void OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	UFUNCTION()
	virtual void OnRep_IsProned();

	void RecalculatePronedEyeHeight();

	//void SetIsTransition(bool bNewIsTransition);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IsProned, Category = Character)
	uint32 bIsProned : 1;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = Character)
	uint32 bIsSlowing : 1;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = Character)
	uint32 bIsSprinting : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float PronedEyeHeight;

private:
	bool CanAim() const;
	void Aim();
	void UnAim();

	bool CanLean() const;

	UFUNCTION(Server, Reliable)
	void Server_Lean(ELeaningDirection NewLeaningDirection);

	bool IsCrawling() const;

	bool CanPerformCrouchAction() const;
	bool CanPerformMoveAction() const;
	bool CanPerformProneAction() const;

	FName GetCharacterWeaponHolsterSocketName(AWeapon* Weapon) const;

	UFUNCTION()
	void Handle_ADSTimelineUpdate(float Value);

	/*UFUNCTION()
	void Handle_ADSTimelineFinished();*/

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceTurningInPlace(ETurningDirection NewTurningDirection);

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceCrouchAimToTransitionIdleLowAimToProneIdleAimStarted();

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceCrouchAimSlowToTransitionIdleLowAimToProneIdleAimStarted();

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceIdleAimToTransitionIdleAimToProneIdleAimStarted();

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceIdleAimToTransitionIdleAimToSprintSlowStarted();

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceIdleLowAimToTransitionIdleLowAimToProneIdleAimStarted();

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceProneFastToTransitionProneIdleAimToIdleAimStarted();

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceProneFastToTransitionProneIdleAimToIdleLowAimStarted();

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceProneIdleAimToTransitionProneIdleAimToIdleAimStarted();

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceProneIdleAimToTransitionProneIdleAimToIdleLowAimStarted();

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceSprintSlowToTransitionSprintSlowToCrouchIdleAimStarted();

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceSprintSlowToTransitionSprintSlowToIdleAimStarted();

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceSprintSlowToTransitionSprintSlowToProneIdleAimStarted();

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceTransitionIdleAimToProneIdleAimToProneIdleAimStarted();

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceTransitionIdleAimToSprintSlowToSprintSlowStarted();

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceTransitionIdleLowAimToProneIdleAimToProneIdleAimStarted();

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceTransitionProneIdleAimToIdleAimToIdleAimStarted();

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceTransitionProneIdleAimToIdleLowAimToIdleLowAimStarted();

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceTransitionSprintSlowToCrouchIdleAimToIdleLowAimStarted();

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceTransitionSprintSlowToIdleAimToIdleAimStarted();

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceTransitionSprintSlowToIdleAimToIdleLowAimStarted();

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceTransitionSprintSlowToProneIdleAimToProneIdleAimStarted();

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceWalkAimToTransitionIdleAimToProneIdleAimStarted();

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceWalkAimSlowToTransitionIdleAimToProneIdleAimStarted();
	
	UFUNCTION()
	void Handle_OnCombatComponentActionEnd(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentActionStart(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentIdle(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentIdleToOut(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentOut(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentOutToIdle(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentOutToIdleArm(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentWeaponChamberCheck(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentWeaponFire(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentWeaponFireDry(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentWeaponFiremode(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentWeaponFiremodeCheck(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentWeaponMagCheck(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentWeaponMagIn(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentWeaponMagOut(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentWeaponReloadCharge(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnHandsAnimInstanceIdle();

	UFUNCTION()
	void Handle_OnHandsAnimInstanceIdleToOut();

	UFUNCTION()
	void Handle_OnHandsAnimInstanceOut();

	UFUNCTION()
	void Handle_OnHandsAnimInstanceOutToIdle();

	UFUNCTION()
	void Handle_OnInventoryComponentWeaponArrayReplicated();

	UFUNCTION()
	void OnRep_LeaningDirection();

	void OnCharacterAimAction(const FInputActionValue& Value);
	void OnCharacterAlterAction(const FInputActionValue& Value);
	void OnCharacterCrouchAction(const FInputActionValue& Value);
	void OnCharacterEquipPrimaryWeaponAction(const FInputActionValue& Value);
	void OnCharacterEquipSecondaryWeaponAction(const FInputActionValue& Value);
	void OnCharacterHolsterWeaponAction(const FInputActionValue& Value);
	void OnCharacterLeanLeftAction(const FInputActionValue& Value);
	void OnCharacterLeanRightAction(const FInputActionValue& Value);
	void OnCharacterLookAction(const FInputActionValue& Value);
	void OnCharacterMoveBackwardAction(const FInputActionValue& Value);
	void OnCharacterMoveForwardAction(const FInputActionValue& Value);
	void OnCharacterMoveLeftAction(const FInputActionValue& Value);
	void OnCharacterMoveRightAction(const FInputActionValue& Value);
	void OnCharacterProneAction(const FInputActionValue& Value);
	void OnCharacterSlowAction(const FInputActionValue& Value);
	void OnCharacterSprintAction(const FInputActionValue& Value);
	void OnWeaponChamberCheckAction(const FInputActionValue& Value);
	void OnWeaponFireAction(const FInputActionValue& Value);
	void OnWeaponFiremodeAction(const FInputActionValue& Value);
	void OnWeaponReloadAction(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void Server_EquipWeaponProgressive(AWeapon* WeaponToEquip);

	UFUNCTION(Server, Reliable)
	void Server_HolsterWeaponProgressive();

	UFUNCTION(Server, Reliable)
	void Server_CheckWeaponChamber();

	UFUNCTION(Server, Reliable)
	void Server_FireWeapon(bool bFire);

	UFUNCTION(Server, Reliable)
	void Server_ChangeWeaponFiremode();

	UFUNCTION(Server, Reliable)
	void Server_CheckWeaponFiremode();

	UFUNCTION(Server, Reliable)
	void Server_CheckWeaponMag();

	UFUNCTION(Server, Reliable)
	void Server_WeaponMagOut();

	/*UFUNCTION(Server, Reliable)
	void Server_SetLeaningDirection(ELeaningDirection NewLeaningDirection);*/

	/*UFUNCTION(Server, Reliable)
	void Server_SetLeanTransitionDuration(float NewLeanTransitionDuration);*/

	/*UFUNCTION(Server, Reliable)
	void Server_SetLeaningRate(float Rate);*/

	void SetRemoteViewYaw(float NewRemoteYaw);
	void UpdateADSCameraTargetLocation();
	void UpdateTargetLeaningAngle();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> HandsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> LegsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ActorComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCharacterCombatComponent> CharacterCombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ActorComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCharacterInventoryComponent> CharacterInventoryComponent;

	UPROPERTY()
	TObjectPtr<UShooterCharacterMovementComponent> ShooterCharacterMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataAsset", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UCharacterDataAsset> CharacterDataAsset;

	UPROPERTY(EditAnywhere, Category = "SkeletalMeshMergeParams")
	FSkeletalMeshMergeParams CharacterMeshMergeParams;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputMapping", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputMappingContext> CombatMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputMapping", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputMappingContext> InventoryMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputMapping", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputMappingContext> MovementMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputMapping", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputMappingContext> OtherMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterAimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterAlterAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterCrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterEquipPrimaryWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterEquipSecondaryWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterHolsterWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterLeanLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterLeanRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterMoveBackwardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterMoveForwardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterMoveLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterMoveRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterProneAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterSlowAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterSprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> WeaponChamberCheckAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> WeaponFireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> WeaponFiremodeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> WeaponReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTimelineComponent> ADSTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UCurveFloat> ADSTimelineCurve;

	UPROPERTY(EditAnywhere, BlueprintReadonly, Category = "Character", meta = (AllowPrivateAccess = "true", ClampMin = "0", UIMin = "0", ClampMax = "45", UIMax = "45", ForceUnits = "deg"))
	float MaxLeaningAngle;

	float DefaultCameraFOV;

	FVector ADSCameraTargetLocation;

	FOnTimelineFloat OnADSTimelineUpdate;
	//FOnTimelineEventStatic OnADSTimelineFinished;

	bool bIsAiming;

	bool bIsAlterAction;

	bool bIsMoveInputBackward;
	bool bIsMoveInputForward;
	bool bIsMoveInputLeft;
	bool bIsMoveInputRight;

	UPROPERTY(Replicated)
	bool bIsTransition;

	//const float DefaultAnimationTransitionDuration = 0.25f;

	UPROPERTY(ReplicatedUsing = OnRep_LeaningDirection)
	ELeaningDirection LeaningDirection;

	float TargetLeaningAngle;

	UPROPERTY(Replicated)
	ETurningDirection TurningDirection;

	/*UPROPERTY(Replicated)
	float LeaningRate;*/

	/*UPROPERTY(Replicated)
	float LeanTransitionDuration;*/

	TObjectPtr<AWeapon> NextWeaponToEquip;

	UPROPERTY(Replicated)
	uint8 RemoteViewYaw;

public:
	//FORCEINLINE float GetDefaultAnimationTransitionDuration() const { return DefaultAnimationTransitionDuration; }
	FORCEINLINE UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }
	FORCEINLINE UCharacterCombatComponent* GetCharacterCombatComponent() const { return CharacterCombatComponent; }
	FORCEINLINE USkeletalMeshComponent* GetHandsMesh() const { return HandsMesh; }
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }
	FORCEINLINE bool GetIsTransition() const { return bIsTransition; }
	FORCEINLINE ELeaningDirection GetLeaningDirection() const { return LeaningDirection; }
	FORCEINLINE ETurningDirection GetTurningDirection() const { return TurningDirection; }
	//FORCEINLINE float GetLeaningRate() const { return LeaningRate; }
	//FORCEINLINE float GetLeanTransitionDuration() const { return LeanTransitionDuration; }
	//FORCEINLINE float GetMaxLean() const { return MaxLean; }
	FORCEINLINE float GetTargetLeaningAngle() const { return TargetLeaningAngle; }

};
