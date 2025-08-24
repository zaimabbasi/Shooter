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
enum class ECombatAction : uint8;
enum class ETurningDirection : uint8;
struct FInputActionValue;

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AShooterCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void RecalculateBaseEyeHeight() override;
	//virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;

	//virtual void Tick(float DeltaTime) override;

	virtual void Init();
	ECombatAction GetCombatAction() const;
	AWeapon* GetEquippedWeapon() const;

	UAnimInstance* GetHandsAnimInstance() const;
	ECombatAction GetHandsAnimCombatAction() const;
	
	virtual float GetAllowedAO_Yaw() const;
	virtual void UpdateAO_Pitch(float ControlRotationPitch, float DeltaTime);
	virtual void UpdateAO_Yaw(float ControlRotationYaw, float DeltaTime);
	virtual void OnMovementUpdated(float DeltaTime, const FVector& OldLocation, const FVector& OldVelocity);

	UFUNCTION()
	virtual void OnRep_IsProned();

	UFUNCTION(BlueprintCallable, Category = "Character", meta = (HidePin = "bClientSimulation"))
	virtual void Prone(bool bClientSimulation = false);

	UFUNCTION(BlueprintCallable, Category = "Character", meta = (HidePin = "bClientSimulation"))
	virtual void UnProne(bool bClientSimulation = false);

	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual bool CanProne() const;

	virtual void OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);
	virtual void OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust);
	void RecalculatePronedEyeHeight();

	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual void Slow();

	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual void UnSlow();

	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual bool CanSlow() const;

	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual void Sprint();

	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual void UnSprint();

	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual bool CanSprint() const;

protected:
	virtual void BeginPlay() override;

	virtual void Aim();
	virtual void UnAim();
	virtual bool CanAim() const;
	virtual bool CanAimInCombatAction(ECombatAction CombatAction) const;
	virtual bool CanLean() const;

	virtual void StartRecoilUpdate();
	virtual void StopRecoilUpdate();

	virtual void StartRecoilRecoveryUpdate();
	virtual void StopRecoilRecoveryUpdate();

	virtual void StartLeaningUpdate();
	virtual void StopLeaningUpdate();

	virtual FName GetCharacterWeaponHolsterSocketName(AWeapon* Weapon) const;

	FRotator CalculateRecoilStep(float DeltaTime) const;
	FRotator CalculateRecoilRecoveryStep(float DeltaTime) const;
	void AdjustRecoilAccumulated(FRotator DeltaControlRotation);
	void AdjustRecoilRecovery(FRotator DeltaControlRotation);
	void OnRecoilUpdateEnd();

	void StartProceduralAnim(bool bHasVelocity) const;
	void StopProceduralAnim() const;
	void StartResetProceduralAnim(float PlayRate = 1.0f) const;

	float GetProceduralAnimScaleValue() const;
	float GetControllerInputScaleValue() const;

	virtual void OnCharacterAimAction(const FInputActionValue& Value);
	virtual void OnCharacterAlterAction(const FInputActionValue& Value);
	virtual void OnCharacterCrouchAction(const FInputActionValue& Value);
	virtual void OnCharacterEquipPrimaryWeaponAction(const FInputActionValue& Value);
	virtual void OnCharacterEquipSecondaryWeaponAction(const FInputActionValue& Value);
	virtual void OnCharacterHolsterWeaponAction(const FInputActionValue& Value);
	virtual void OnCharacterLeanLeftAction(const FInputActionValue& Value);
	virtual void OnCharacterLeanRightAction(const FInputActionValue& Value);
	virtual void OnCharacterLookAction(const FInputActionValue& Value);
	virtual void OnCharacterMoveBackwardAction(const FInputActionValue& Value);
	virtual void OnCharacterMoveForwardAction(const FInputActionValue& Value);
	virtual void OnCharacterMoveLeftAction(const FInputActionValue& Value);
	virtual void OnCharacterMoveRightAction(const FInputActionValue& Value);
	virtual void OnCharacterProneAction(const FInputActionValue& Value);
	virtual void OnCharacterSlowAction(const FInputActionValue& Value);
	virtual void OnCharacterSprintAction(const FInputActionValue& Value);
	virtual void OnWeaponChamberCheckAction(const FInputActionValue& Value);
	virtual void OnWeaponFireAction(const FInputActionValue& Value);
	virtual void OnWeaponFiremodeAction(const FInputActionValue& Value);
	virtual void OnWeaponReloadAction(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void Server_Lean(ELeaningDirection NewLeaningDirection);
	virtual void Server_Lean_Implementation(ELeaningDirection NewLeaningDirection);

	UFUNCTION()
	virtual void OnRep_LeaningDirection(ELeaningDirection OldLeaningDirection);

	UFUNCTION()
	virtual void Handle_OnADSTimelineUpdate(float Value);

	/*UFUNCTION()
	virtual void Handle_OnADSTimelineFinished();*/

	UFUNCTION()
	virtual void Handle_OnIdleAnimTimelineHorizontalMovementUpdate(float Value);

	UFUNCTION()
	virtual void Handle_OnIdleAnimTimelineVerticalMovementUpdate(float Value);

	UFUNCTION()
	virtual void Handle_OnIdleAnimTimelineRollRotationUpdate(float Value);

	UFUNCTION()
	virtual void Handle_OnWalkAnimTimelineHorizontalMovementUpdate(float Value);

	UFUNCTION()
	virtual void Handle_OnWalkAnimTimelineVerticalMovementUpdate(float Value);

	UFUNCTION()
	virtual void Handle_OnWalkAnimTimelineRollRotationUpdate(float Value);

	UFUNCTION()
	virtual void Handle_OnIdleWalkAnimResetTimelineUpdate(float Value);

	UFUNCTION()
	virtual void Handle_OnIdleWalkAnimResetTimelineFinished();

	UFUNCTION()
	virtual void Handle_OnTurningAnimTimelineUpdate(float Value);

	UFUNCTION()
	virtual void Handle_OnTurningAnimTimelineFinished();

	UFUNCTION()
	virtual bool Handle_OnRecoilUpdate(float DeltaTime);

	UFUNCTION()
	virtual bool Handle_OnRecoilRecoveryUpdate(float DeltaTime);

	UFUNCTION()
	virtual bool Handle_OnLeaningUpdate(float DeltaTime);

	/*UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceIdle();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceWalk();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceThirdOrCombatAction();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceTurningInPlace(ETurningDirection NewTurningDirection);*/

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceCrouchAimToTransitionIdleLowAimToProneIdleAimStarted();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceCrouchAimSlowToTransitionIdleLowAimToProneIdleAimStarted();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceIdleAimToTransitionIdleAimToProneIdleAimStarted();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceIdleAimToTransitionIdleAimToSprintSlowStarted();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceIdleLowAimToTransitionIdleLowAimToProneIdleAimStarted();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceProneFastToTransitionProneIdleAimToIdleAimStarted();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceProneFastToTransitionProneIdleAimToIdleLowAimStarted();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceProneIdleAimToTransitionProneIdleAimToIdleAimStarted();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceProneIdleAimToTransitionProneIdleAimToIdleLowAimStarted();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceSprintSlowToTransitionSprintSlowToCrouchIdleAimStarted();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceSprintSlowToTransitionSprintSlowToIdleAimStarted();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceSprintSlowToTransitionSprintSlowToProneIdleAimStarted();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceTransitionIdleAimToProneIdleAimToProneIdleAimStarted();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceTransitionIdleAimToSprintSlowToSprintSlowStarted();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceTransitionIdleLowAimToProneIdleAimToProneIdleAimStarted();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceTransitionProneIdleAimToIdleAimToIdleAimStarted();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceTransitionProneIdleAimToIdleLowAimToIdleLowAimStarted();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceTransitionSprintSlowToCrouchIdleAimToIdleLowAimStarted();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceTransitionSprintSlowToIdleAimToIdleAimStarted();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceTransitionSprintSlowToIdleAimToIdleLowAimStarted();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceTransitionSprintSlowToProneIdleAimToProneIdleAimStarted();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceWalkAimToTransitionIdleAimToProneIdleAimStarted();

	UFUNCTION()
	virtual void Handle_OnCharacterAnimInstanceWalkAimSlowToTransitionIdleAimToProneIdleAimStarted();

	UFUNCTION()
	virtual void Handle_OnCharacterCombatCombatActionChanged(ECombatAction CombatAction);

	UFUNCTION()
	virtual void Handle_OnCharacterCombatEquippedWeaponChanged(AWeapon* Weapon);

	UFUNCTION()
	virtual void Handle_OnCharacterInventoryWeaponArrayReplicated();

	UFUNCTION()
	virtual void Handle_OnCharacterCombatWeaponRecoilGenerated(AWeapon* Weapon, float RecoilHorizontalKick, float RecoilVerticalKick);

	UFUNCTION()
	virtual void Handle_OnMovementComponentSprint();

private:
	void CalculateADSCameraTargetLocation();
	void CalculateLeaningTargetAngle();
	void CalculateLeaningTransitionDuration(ELeaningDirection OldLeaningDirection);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float PronedEyeHeight;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IsProned, Category = Character)
	uint32 bIsProned : 1;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = Character)
	uint32 bIsSlowing : 1;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = Character)
	uint32 bIsSprinting : 1;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UCharacterDataAsset> CharacterDataAsset;

	UPROPERTY(EditAnywhere)
	FSkeletalMeshMergeParams CharacterMeshMergeParams;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputMappingContext> CombatMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputMappingContext> InventoryMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputMappingContext> MovementMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputMappingContext> OtherMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterAimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterAlterAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterCrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterEquipPrimaryWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterEquipSecondaryWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterHolsterWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterLeanLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterLeanRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterMoveBackwardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterMoveForwardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterMoveLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterMoveRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterProneAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterSlowAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterSprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> WeaponChamberCheckAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> WeaponFireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> WeaponFiremodeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> WeaponReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTimelineComponent> ADSTimeline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTimelineComponent> IdleAnimTimeline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTimelineComponent> WalkAnimTimeline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTimelineComponent> IdleWalkAnimResetTimeline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTimelineComponent> TurningAnimTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UCurveFloat> ADSTimelineCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UCurveFloat> IdleAnimTimelineHorizontalMovementCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UCurveFloat> IdleAnimTimelineVerticalMovementCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UCurveFloat> IdleAnimTimelineRollRotationCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UCurveFloat> WalkAnimTimelineHorizontalMovementCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UCurveFloat> WalkAnimTimelineVerticalMovementCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UCurveFloat> WalkAnimTimelineRollRotationCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UCurveFloat> IdleWalkAnimResetTimelineCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UCurveFloat> TurningAnimTimelineCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float IdleAnimMaxHorizontalMovement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float IdleAnimMinHorizontalMovement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float IdleAnimMaxVerticalMovement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float IdleAnimMinVerticalMovement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float IdleAnimMaxRollRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float IdleAnimMinRollRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float WalkAnimMaxHorizontalMovement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float WalkAnimMinHorizontalMovement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float WalkAnimMaxVerticalMovement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float WalkAnimMinVerticalMovement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float WalkAnimMaxRollRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float WalkAnimMinRollRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float SwayHorizontalMovementLimit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float SwayVerticalMovementLimit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float SwayRollRotationLimit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float SwayHorizontalMovementSensitivity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float SwayVerticalMovementSensitivity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float SwayRollRotationSensitivity;

	FOnTimelineFloat OnADSTimelineUpdate;
	//FOnTimelineEventStatic OnADSTimelineFinished;

	FOnTimelineFloat OnIdleAnimTimelineHorizontalMovementUpdate;
	FOnTimelineFloat OnIdleAnimTimelineVerticalMovementUpdate;
	FOnTimelineFloat OnIdleAnimTimelineRollRotationUpdate;

	FOnTimelineFloat OnWalkAnimTimelineHorizontalMovementUpdate;
	FOnTimelineFloat OnWalkAnimTimelineVerticalMovementUpdate;
	FOnTimelineFloat OnWalkAnimTimelineRollRotationUpdate;

	FOnTimelineFloat OnIdleWalkAnimResetTimelineUpdate;
	FOnTimelineEventStatic OnIdleWalkAnimResetTimelineFinished;

	FOnTimelineFloat OnTurningAnimTimelineUpdate;
	FOnTimelineEventStatic OnTurningAnimTimelineFinished;

	FTickerDelegate OnRecoilUpdate;
	FTickerDelegate OnRecoilRecoveryUpdate;

	FTSTicker::FDelegateHandle OnRecoilUpdateHandle;
	FTSTicker::FDelegateHandle OnRecoilRecoveryUpdateHandle;

	FTickerDelegate OnLeaningUpdate;
	FTSTicker::FDelegateHandle OnLeaningUpdateHandle;

	float ProceduralAnimHorizontalMovement;
	float ProceduralAnimVerticalMovement;
	float ProceduralAnimRollRotation;

	float RecoilHorizontal;
	float RecoilVertical;

	float RecoilHorizontalAccumulated;
	float RecoilVerticalAccumulated;

	float RecoilRecoveryHorizontal;
	float RecoilRecoveryVertical;

	float ControllerHorizontalVelocity;
	float ControllerVerticalVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true", UIMin = "0", UIMax = "1", ClampMin = "0", ClampMax = "1", Units = "s"))
	float RecoilKickTotalTime;

	UPROPERTY(EditAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true", UIMin = "0", UIMax = "1", ClampMin = "0", ClampMax = "1", Units = "s"))
	float RecoilRecoveryTotalTime;
	
	FRotator RecoilLastControlRotation;

	UPROPERTY(EditAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true", ClampMin = "0", ClampMax = "45", Units = "deg"))
	float LeaningMaxAngle;

	UPROPERTY(EditAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true", ClampMin = "1", ClampMax = "10.0"))
	float LeaningInterpSpeed;

	float LeaningTargetAngle;

	UPROPERTY(EditAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true", ClampMin = "0", ClampMax = "1.0", Units = "s"))
	float LeaningDefaultTransitionDuration;

	float LeaningTransitionDuration;

	float CameraDefaultFOV;

	FVector ADSCameraTargetLocation;

	bool bIsAiming;

	UPROPERTY(EditAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess = "true", UIMin = "0", UIMax = "1", ClampMin = "0", ClampMax = "1"))
	float ControllerInputAimingScale;

	bool bIsAlterAction;

	bool bIsMoveInputBackward;
	bool bIsMoveInputForward;
	bool bIsMoveInputLeft;
	bool bIsMoveInputRight;

	UPROPERTY(Replicated)
	bool bIsTransition;

	UPROPERTY(ReplicatedUsing = OnRep_LeaningDirection)
	ELeaningDirection LeaningDirection;

	float LeaningAngle;

	UPROPERTY(Replicated)
	ETurningDirection TurningDirection;

	UPROPERTY(Replicated)
	float AO_Pitch;

	UPROPERTY(Replicated)
	float AO_Yaw;

	UPROPERTY(Replicated)
	float RootBoneYaw;

	float ReferenceActorRotationYaw;
	float VelocityYawOffset;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> HandsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> LegsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCharacterCombatComponent> CharacterCombat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCharacterInventoryComponent> CharacterInventory;

	UPROPERTY()
	TObjectPtr<UShooterCharacterMovementComponent> ShooterCharacterMovement;

public:
	FORCEINLINE UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }
	FORCEINLINE USkeletalMeshComponent* GetHandsMesh() const { return HandsMesh; }
	FORCEINLINE UCharacterCombatComponent* GetCharacterCombat() const { return CharacterCombat; }
	FORCEINLINE UCharacterInventoryComponent* GetCharacterInventory() const { return CharacterInventory; }
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }
	FORCEINLINE bool GetIsTransition() const { return bIsTransition; }
	FORCEINLINE ELeaningDirection GetLeaningDirection() const { return LeaningDirection; }
	FORCEINLINE ETurningDirection GetTurningDirection() const { return TurningDirection; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetLeaningAngle() const { return LeaningAngle; }
	FORCEINLINE float GetRootBoneYaw() const { return RootBoneYaw; }
	FORCEINLINE float GetVelocityYawOffset() const { return VelocityYawOffset; }
	FORCEINLINE float GetLeaningInterpSpeed() const { return LeaningInterpSpeed; }
	FORCEINLINE float GetLeaningTargetAngle() const { return LeaningTargetAngle; }
	FORCEINLINE float GetLeaningTransitionDuration() const { return LeaningTransitionDuration; }
	FORCEINLINE float GetSwayHorizontalMovementLimit() const { return SwayHorizontalMovementLimit; }
	FORCEINLINE float GetSwayVerticalMovementLimit() const { return SwayVerticalMovementLimit; }
	FORCEINLINE float GetSwayRollRotationLimit() const { return SwayRollRotationLimit; }
	FORCEINLINE float GetSwayHorizontalMovementSensitivity() const { return SwayHorizontalMovementSensitivity; }
	FORCEINLINE float GetSwayVerticalMovementSensitivity() const { return SwayVerticalMovementSensitivity; }
	FORCEINLINE float GetSwayRollRotationSensitivity() const { return SwayRollRotationSensitivity; }
	FORCEINLINE float GetProceduralAnimHorizontalMovement() const { return ProceduralAnimHorizontalMovement; }
	FORCEINLINE float GetProceduralAnimVerticalMovement() const { return ProceduralAnimVerticalMovement; }
	FORCEINLINE float GetProceduralAnimRollRotation() const { return ProceduralAnimRollRotation; }
	FORCEINLINE float GetRecoilRecoveryTotalTime() const { return RecoilRecoveryTotalTime; }

};
