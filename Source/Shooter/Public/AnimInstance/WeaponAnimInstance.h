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

	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceIdle;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceIdleToOut;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceOut;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceOutToIdle;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstancePatronInWeapon;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceFiremode;
	FOnWeaponAnimInstanceAnimNotifySignature OnWeaponAnimInstanceWeaponSelector;

protected:
	UFUNCTION()
	void AnimNotify_Idle() const;

	UFUNCTION()
	void AnimNotify_IdleToOut() const;

	UFUNCTION()
	void AnimNotify_Out() const;

	UFUNCTION()
	void AnimNotify_OutToIdle() const;

	UFUNCTION()
	void AnimNotify_PatronInWeapon() const;

	UFUNCTION()
	void AnimNotify_Firemode() const;

	UFUNCTION()
	void AnimNotify_WeaponSelector() const;

private:
	TObjectPtr<AWeapon> Weapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	ECombatAction CombatAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	bool bIsHolster;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	EWeaponFiremode Firemode;

};
