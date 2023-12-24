// Fill out your copyright notice in the Description page of Project Settings.


#include "HandsAnimInstance.h"
#include "ShooterCharacter.h"

void UHandsAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UHandsAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}
	if (ShooterCharacter == nullptr)
	{
		return;
	}

	if (USkeletalMeshComponent* HandsMesh1P = ShooterCharacter->GetHandsMesh1P())
	{
		RelativeRotation = HandsMesh1P->GetRelativeRotation();
	}
	ControlRotationPitch = ShooterCharacter->GetControlRotation().Pitch;
}
