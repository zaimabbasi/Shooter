// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WeaponAnimInstance.generated.h"

class AShooterCharacter;
class AWeapon;
enum class EWeaponAction : uint8;

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
	TObjectPtr<AWeapon> Weapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	EWeaponAction WeaponAction;

};
