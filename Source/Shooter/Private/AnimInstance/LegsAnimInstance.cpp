// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/LegsAnimInstance.h"
#include "Character/ShooterCharacter.h"

void ULegsAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void ULegsAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
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

	CharacterMesh = ShooterCharacter->GetMesh();

}
