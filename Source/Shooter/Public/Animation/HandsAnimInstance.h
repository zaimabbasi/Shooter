// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HandsAnimInstance.generated.h"

class AShooterCharacter;
enum class ECombatAction : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHandsAnimInstanceAnimNotifySignature);

UCLASS()
class SHOOTER_API UHandsAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	UFUNCTION()
	void AnimNotify_Idle() const;

	UFUNCTION()
	void AnimNotify_IdleToOut() const;

	UFUNCTION()
	void AnimNotify_Out() const;

	UFUNCTION()
	void AnimNotify_OutToIdle() const;

private:
	void CalculateRootBoneLocation();
	void CalculateWeaponRootAnimTransform();
	void CalculateBendGoalLeftTransform();
	void CalculateBendGoalRightTransform();
	void CalculateWeaponLCollarboneMarkerTransform();
	void CalculateWeaponRCollarboneMarkerTransform();
	void CalculateWeaponLHandMarkerTransform();
	void CalculateWeaponRHandMarkerTransform();
	void CalculateTransforms();

public:
	FOnHandsAnimInstanceAnimNotifySignature OnHandsAnimInstanceIdle;
	FOnHandsAnimInstanceAnimNotifySignature OnHandsAnimInstanceIdleToOut;
	FOnHandsAnimInstanceAnimNotifySignature OnHandsAnimInstanceOut;
	FOnHandsAnimInstanceAnimNotifySignature OnHandsAnimInstanceOutToIdle;

protected:
	TObjectPtr<AShooterCharacter> ShooterCharacter;
	TObjectPtr<USkeletalMeshComponent> HandsMesh;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> CharacterMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsAiming;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsProned;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsSprinting;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsTransition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bHasVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsThirdAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AO_Pitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AO_Yaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float LeaningAngle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ECombatAction CombatAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector RootBoneLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTransform WeaponRootAnimTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTransform BendGoalLeftTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTransform BendGoalRightTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTransform WeaponLCollarboneMarkerTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTransform WeaponRCollarboneMarkerTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTransform WeaponLHandMarkerTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTransform WeaponRHandMarkerTransform;

public:
	FORCEINLINE ECombatAction GetCombatAction() const { return CombatAction; }

};
