// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/WeaponAnimInstance.h"
#include "Actor/Weapon.h"
#include "Character/ShooterCharacter.h"

void UWeaponAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Weapon = Cast<AWeapon>(GetOwningActor());

	if (Weapon)
	{
		CombatAction = Weapon->GetCombatAction();
	}
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

	CombatAction = Weapon->GetCombatAction();

}

void UWeaponAnimInstance::AnimNotify_Idle() const
{
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));
	OnWeaponAnimInstanceIdle.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_IdleToOut() const
{
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));
	OnWeaponAnimInstanceIdleToOut.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_Out() const
{
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));
	OnWeaponAnimInstanceOut.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_OutToIdle() const
{
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));
	OnWeaponAnimInstanceOutToIdle.Broadcast();
}
