// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/WeaponAnimInstance.h"
#include "Actor/Weapon.h"
#include "Character/ShooterCharacter.h"

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

	WeaponAction = Weapon->GetWeaponAction();

}

bool UWeaponAnimInstance::HandleNotify(const FAnimNotifyEvent& AnimNotifyEvent)
{
	Super::HandleNotify(AnimNotifyEvent);

	if (Weapon == nullptr)
	{
		return false;
	}
	return Weapon->HandleAnimNotify(AnimNotifyEvent);
}
