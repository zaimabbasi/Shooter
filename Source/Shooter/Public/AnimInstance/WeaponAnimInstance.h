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

	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceActionEnd;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceActionStart;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceChamberCheck;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceFire;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceFireDry;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceFiremode;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceFiremodeCheck;
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
	void AnimNotify_WeaponHammer() const;

	UFUNCTION()
	void AnimNotify_ShellPort() const;

private:
	TObjectPtr<ACharacter> OwningCharacter;
	TObjectPtr<AWeapon> Weapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> CharacterMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	ECombatAction CombatAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	bool bIsHolster;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	bool bHasPatronInWeaponAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	EWeaponFiremode Firemode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	float FireAnimPlayRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	bool bLoopFireAnim;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	FTransform WeaponRootAnimTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	FTransform LCollarboneTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	FTransform RCollarboneTransform;

};
