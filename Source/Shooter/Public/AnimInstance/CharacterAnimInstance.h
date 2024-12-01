// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

class AShooterCharacter;
enum class ELeanDirection: uint8;
enum class ETurnDirection: uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterAnimInstanceAnimNotifySignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterAnimInstanceControllerDesiredRotationNeededSignature, bool, bControllerDesiredRotationNeeded);

UCLASS()
class SHOOTER_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	// Delegates - From Standing To Transitions
	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceIdleAimToTransitionIdleAimToSprintSlowStarted;
	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceIdleAimToTransitionIdleAimToProneIdleAimStarted;
	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceSprintSlowToTransitionSprintSlowToCrouchIdleAimStarted;
	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceSprintSlowToTransitionSprintSlowToIdleAimStarted;
	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceSprintSlowToTransitionSprintSlowToProneIdleAimStarted;
	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceWalkAimToTransitionIdleAimToProneIdleAimStarted;
	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceWalkAimSlowToTransitionIdleAimToProneIdleAimStarted;

	// Delegates - From Crouching To Transitions
	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceCrouchAimToTransitionIdleLowAimToProneIdleAimStarted;
	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceCrouchAimSlowToTransitionIdleLowAimToProneIdleAimStarted;
	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceIdleLowAimToTransitionIdleLowAimToProneIdleAimStarted;

	// Delegates - From Proning To Transitions
	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceProneFastToTransitionProneIdleAimToIdleAimStarted;
	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceProneFastToTransitionProneIdleAimToIdleLowAimStarted;
	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceProneIdleAimToTransitionProneIdleAimToIdleAimStarted;
	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceProneIdleAimToTransitionProneIdleAimToIdleLowAimStarted;

	// Delegates - From Transitions
	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceTransitionIdleAimToProneIdleAimToProneIdleAimStarted;
	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceTransitionIdleAimToSprintSlowToSprintSlowStarted;
	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceTransitionIdleLowAimToProneIdleAimToProneIdleAimStarted;
	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceTransitionProneIdleAimToIdleAimToIdleAimStarted;
	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceTransitionProneIdleAimToIdleLowAimToIdleLowAimStarted;
	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceTransitionSprintSlowToCrouchIdleAimToIdleLowAimStarted;
	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceTransitionSprintSlowToIdleAimToIdleAimStarted;
	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceTransitionSprintSlowToIdleAimToIdleLowAimStarted;
	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceTransitionSprintSlowToProneIdleAimToProneIdleAimStarted;

	// Delegates - Other
	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceTurnInPlace;
	FOnCharacterAnimInstanceControllerDesiredRotationNeededSignature OnCharacterAnimInstanceControllerDesiredRotationNeeded;

protected:
	// Callbacks - From Standing To Transitions
	UFUNCTION()
	void AnimNotify_IdleAimToTransitionIdleAimToSprintSlowStarted() const;

	UFUNCTION()
	void AnimNotify_IdleAimToTransitionIdleAimToProneIdleAimStarted() const;

	UFUNCTION()
	void AnimNotify_SprintSlowToTransitionSprintSlowToCrouchIdleAimStarted() const;

	UFUNCTION()
	void AnimNotify_SprintSlowToTransitionSprintSlowToIdleAimStarted() const;

	UFUNCTION()
	void AnimNotify_SprintSlowToTransitionSprintSlowToProneIdleAimStarted() const;

	UFUNCTION()
	void AnimNotify_WalkAimToTransitionIdleAimToProneIdleAimStarted() const;

	UFUNCTION()
	void AnimNotify_WalkAimSlowToTransitionIdleAimToProneIdleAimStarted() const;

	// Callbacks - From Crouching To Transitions
	UFUNCTION()
	void AnimNotify_CrouchAimToTransitionIdleLowAimToProneIdleAimStarted() const;

	UFUNCTION()
	void AnimNotify_CrouchAimSlowToTransitionIdleLowAimToProneIdleAimStarted() const;

	UFUNCTION()
	void AnimNotify_IdleLowAimToTransitionIdleLowAimToProneIdleAimStarted() const;

	// Callbacks - From Proning To Transitions
	UFUNCTION()
	void AnimNotify_ProneFastToTransitionProneIdleAimToIdleAimStarted() const;

	UFUNCTION()
	void AnimNotify_ProneFastToTransitionProneIdleAimToIdleLowAimStarted() const;

	UFUNCTION()
	void AnimNotify_ProneIdleAimToTransitionProneIdleAimToIdleAimStarted() const;

	UFUNCTION()
	void AnimNotify_ProneIdleAimToTransitionProneIdleAimToIdleLowAimStarted() const;

	// Callbacks - From Transitions
	UFUNCTION()
	void AnimNotify_TransitionIdleAimToProneIdleAimToProneIdleAimStarted() const;

	UFUNCTION()
	void AnimNotify_TransitionIdleAimToSprintSlowToSprintSlowStarted() const;

	UFUNCTION()
	void AnimNotify_TransitionIdleLowAimToProneIdleAimToProneIdleAimStarted() const;

	UFUNCTION()
	void AnimNotify_TransitionProneIdleAimToIdleAimToIdleAimStarted() const;

	UFUNCTION()
	void AnimNotify_TransitionProneIdleAimToIdleLowAimToIdleLowAimStarted() const;

	UFUNCTION()
	void AnimNotify_TransitionSprintSlowToCrouchIdleAimToIdleLowAimStarted() const;

	UFUNCTION()
	void AnimNotify_TransitionSprintSlowToIdleAimToIdleAimStarted() const;

	UFUNCTION()
	void AnimNotify_TransitionSprintSlowToIdleAimToIdleLowAimStarted() const;

	UFUNCTION()
	void AnimNotify_TransitionSprintSlowToProneIdleAimToProneIdleAimStarted() const;

	// Callbacks - Other
	UFUNCTION()
	void AnimNotify_TurnInPlace() const;

private:
	TObjectPtr<AShooterCharacter> ShooterCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> HandsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	ETurnDirection TurnDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	ELeanDirection LeanDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	FTransform LCollarboneTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	FTransform RCollarboneTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	float AO_Yaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	float AO_Pitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	bool bIsCrouched;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	bool bIsProned;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	bool bIsSlowing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	bool bIsSprinting;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	bool bIsThirdAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	bool bIsWeaponEquipped;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	bool bIsEquippedWeaponPistol;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	bool bIsEquippedWeaponOneHanded;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	float LeanTransitionDuration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	float AnimationTransitionDuration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	bool bHasVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	float VelocityYawOffset;

};
