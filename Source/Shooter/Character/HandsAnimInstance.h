// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "HandsAnimInstance.generated.h"

class AShooterCharacter;
/**
 * 
 */
UCLASS()
class SHOOTER_API UHandsAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation();

	virtual void NativeUpdateAnimation(float DeltaSeconds);

protected:

private:
	AShooterCharacter* ShooterCharacter;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"));
	FRotator RelativeRotation;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"));
	float ControlRotationPitch;
};
