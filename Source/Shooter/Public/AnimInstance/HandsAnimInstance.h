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

	FOnHandsAnimInstanceAnimNotifySignature OnHandsAnimInstanceIdle;
	FOnHandsAnimInstanceAnimNotifySignature OnHandsAnimInstanceIdleToOut;
	FOnHandsAnimInstanceAnimNotifySignature OnHandsAnimInstanceOut;
	FOnHandsAnimInstanceAnimNotifySignature OnHandsAnimInstanceOutToIdle;

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
	TObjectPtr<AShooterCharacter> ShooterCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	float AO_Yaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	float AO_Pitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	float Lean;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	ECombatAction CombatAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	bool bIsWeaponEquipped;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

};
