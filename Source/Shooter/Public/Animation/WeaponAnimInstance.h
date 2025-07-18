// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WeaponAnimInstance.generated.h"

class AShooterCharacter;
class AWeapon;
enum class ECombatAction : uint8;
enum class EWeaponFiremode : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponAnimInstanceAnimNotifySignature);

UCLASS()
class SHOOTER_API UWeaponAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

	void SetCombatAction(ECombatAction NewCombatAction);

protected:
	UFUNCTION()
	void AnimNotify_ActionEnd() const;

	UFUNCTION()
	void AnimNotify_ActionStart() const;

	UFUNCTION()
	void AnimNotify_ChamberCheck() const;

	UFUNCTION()
	void AnimNotify_Fire() const;

	UFUNCTION()
	void AnimNotify_FireDry() const;

	UFUNCTION()
	void AnimNotify_Firemode() const;

	UFUNCTION()
	void AnimNotify_FiremodeCheck() const;

	UFUNCTION()
	void AnimNotify_HandsInHalf1() const;

	UFUNCTION()
	void AnimNotify_HandsInHalf2() const;

	UFUNCTION()
	void AnimNotify_HandsOutHalf1() const;

	UFUNCTION()
	void AnimNotify_HandsOutHalf2() const;

	UFUNCTION()
	void AnimNotify_Idle() const;

	UFUNCTION()
	void AnimNotify_IdleToOut() const;

	UFUNCTION()
	void AnimNotify_MagCheck() const;

	UFUNCTION()
	void AnimNotify_MagIn() const;

	UFUNCTION()
	void AnimNotify_MagOut() const;

	UFUNCTION()
	void AnimNotify_Out() const;

	UFUNCTION()
	void AnimNotify_OutToIdle() const;

	UFUNCTION()
	void AnimNotify_OutToIdleArm() const;

	UFUNCTION()
	void AnimNotify_ReloadCharge() const;

	UFUNCTION()
	void AnimNotify_PatronInWeapon() const;

	UFUNCTION()
	void AnimNotify_WeaponSelector() const;

	UFUNCTION()
	void AnimNotify_WeaponHammer();

	UFUNCTION()
	void AnimNotify_ShellPort() const;

	UFUNCTION()
	void AnimNotify_LHandIn();

	UFUNCTION()
	void AnimNotify_LHandOut();

	UFUNCTION()
	virtual bool Handle_OnLHandInUpdate(float DeltaSeconds);

	UFUNCTION()
	virtual bool Handle_OnLHandOutUpdate(float DeltaSeconds);

	void StartLHandInUpdate();
	void StopLHandInUpdate();
	void StartLHandOutUpdate();
	void StopLHandOutUpdate();

private:
	bool ShouldCopyCharacterIKSLPalm() const;
	void CalculateLHandMarkerAlpha();
	void CalculateSway(float DeltaSeconds);
	void CalculateRecoil(float DeltaSeconds);
	void CalculateRootBoneLocation();
	void CalculateLPalmTransform();
	void CalculateThirdActionTransforms();
	void CalculateWeaponRootAnimTransform();
	void CalculateTransforms();
	//float CalculateVelocityYawOffsetAlpha(float VelocityYawOffset);

public:
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceActionEnd;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceActionStart;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceChamberCheck;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceFire;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceFireDry;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceFiremode;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceFiremodeCheck;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceHandsInHalf1;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceHandsInHalf2;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceHandsOutHalf1;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceHandsOutHalf2;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceIdle;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceIdleToOut;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceMagCheck;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceMagIn;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceMagOut;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceOut;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceOutToIdle;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceOutToIdleArm;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceReloadCharge;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstancePatronInWeapon;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceWeaponSelector;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceWeaponHammer;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceShellPort;

protected:
	TObjectPtr<AWeapon> Weapon;
	TObjectPtr<AShooterCharacter> ShooterCharacter;
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;
	FRotator ControlRotation;
	FRotator ControlRotationLast;
	bool bIsCharacterThirdActionLast;
	float IKBlendDuration;
	float WeaponLHandMarkerAlphaLast;
	//float ProceduralAnimHorizontalMovement;
	//float ProceduralAnimVerticalMovement;
	//float ProceduralAnimRollRotation;
	float SwayHorizontalMovement;
	float SwayVerticalMovement;
	float SwayRollRotation;
	float SwayInterpSpeed;

	bool bIsRecoilKick;
	bool bIsRecoilRecovery;
	float RecoilKickTotalTime;
	float RecoilRecoveryTotalTime;
	float RecoilKickMovement;
	float RecoilKickPitchRotation;

	FTickerDelegate OnLHandInUpdate;
	FTickerDelegate OnLHandOutUpdate;

	FTSTicker::FDelegateHandle OnLHandInUpdateHandle;
	FTSTicker::FDelegateHandle OnLHandOutUpdateHandle;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> CharacterMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> ForegripHandguardMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	ECombatAction CombatAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EWeaponFiremode Firemode;

	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsHolster;*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bHasForegripHandguardMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsPistol;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsOneHanded;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float FireAnimPlayRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bLoopFireAnim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AO_Pitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AO_Yaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector RootBoneLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTransform BendGoalLeftTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTransform BendGoalRightTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTransform LCollarboneTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTransform RCollarboneTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTransform LPalmTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTransform RPalmTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTransform WeaponRootAnimTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bHasCharacterVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsCharacterProned;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsCharacterSprinting;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsCharacterThirdAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float WeaponLHandMarkerAlpha;

public:
	FORCEINLINE ECombatAction GetCombatAction() const { return CombatAction; }

};
