// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterAnimInstance.generated.h"

class AShooterCharacter;
enum class ECharacterStance : uint8;
enum class ELeanDirection: uint8;
enum class ETurnDirection: uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterAnimInstanceAnimNotifySignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterAnimInstanceControllerDesiredRotationNeededSignature, bool, bControllerDesiredRotationNeeded);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterAnimInstanceMaxWalkSpeedNeedChangedSignature);

UCLASS()
class SHOOTER_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	FOnCharacterAnimInstanceAnimNotifySignature OnCharacterAnimInstanceTurnInPlace;
	FOnCharacterAnimInstanceControllerDesiredRotationNeededSignature OnCharacterAnimInstanceControllerDesiredRotationNeeded;
	//FOnCharacterAnimInstanceMaxWalkSpeedNeedChangedSignature OnCharacterAnimInstanceMaxWalkSpeedNeedChanged;

protected:
	UFUNCTION()
	void AnimNotify_TurnInPlace() const;

private:
	TObjectPtr<AShooterCharacter> ShooterCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> HandsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	FTransform LCollarboneTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	FTransform RCollarboneTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	float AO_Yaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	float AO_Pitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	ETurnDirection TurnDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	bool bIsToggleSlow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	bool bIsToggleSprint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	ELeanDirection LeanDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	float LeanTransitionDuration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	float AnimationTransitionDuration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	ECharacterStance CurrentStance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	float AccelerationYawOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	bool bHasVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	float VelocityYawOffset;

};
