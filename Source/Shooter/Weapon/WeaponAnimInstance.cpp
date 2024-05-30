// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponAnimInstance.h"
#include "Shooter/Character/ShooterCharacter.h"
#include "Weapon.h"

void UWeaponAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Weapon = Cast<AWeapon>(GetOwningActor());

}

void UWeaponAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Weapon == nullptr)
	{
		Weapon = Cast<AWeapon>(GetOwningActor());
	}
	if (Weapon == nullptr)
	{
		return;
	}

}
