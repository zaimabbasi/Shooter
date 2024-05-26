// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponAnimInstance.h"
#include "Shooter/Character/ShooterCharacter.h"
#include "Shooter/Data/WeaponDataAsset.h"
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

	if (UWeaponAnimationDataAsset* WeaponAnimationDataAsset = Weapon->GetWeaponAnimationDataAsset())
	{
		IdleAnim = WeaponAnimationDataAsset->IdleAnim;
		ActionEndAnim = WeaponAnimationDataAsset->ActionEndAnim;
		ActionStartAnim = WeaponAnimationDataAsset->ActionStartAnim;
		MagInAnim = WeaponAnimationDataAsset->MagInAnim;
		MagOutAnim = WeaponAnimationDataAsset->MagOutAnim;
		ReloadChargeAnim = WeaponAnimationDataAsset->ReloadChargeAnim;
	}

}
