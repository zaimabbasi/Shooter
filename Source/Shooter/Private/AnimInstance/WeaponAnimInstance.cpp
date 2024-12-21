// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/WeaponAnimInstance.h"
#include "Actor/Weapon.h"
#include "Struct/ShooterUtility.h"
#include "Type/ShooterNameType.h"

void UWeaponAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Weapon = Cast<AWeapon>(GetOwningActor());

	if (Weapon)
	{
		CombatAction = Weapon->GetCombatAction();
		//FireAnimPlayRate = Weapon->GetRateOfFire() / 60.0f;
		FireAnimPlayRate = 1.0f;
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

	USkeletalMeshComponent* WeaponMesh = Weapon->GetMesh();
	CharacterMesh = Weapon->GetShooterCharacterOwnerMesh();
	CombatAction = Weapon->GetCombatAction();
	bHasPatronInWeaponAmmo = Weapon->HasPatronInWeaponAmmo();
	Firemode = Weapon->GetFiremode();
	bIsHolster = Weapon->GetIsHolster();
	bIsThirdAction = Weapon->IsThirdAction();

	if (bIsThirdAction && CharacterMesh)
	{
		BendGoalLeftTransform = CharacterMesh->GetSocketTransform(BEND_GOAL_LEFT_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		BendGoalRightTransform = CharacterMesh->GetSocketTransform(BEND_GOAL_RIGHT_SOCKET_NAME, ERelativeTransformSpace::RTS_World);

		LPalmTransform = CharacterMesh->GetSocketTransform(IK_S_L_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		RPalmTransform = CharacterMesh->GetSocketTransform(IK_S_R_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);

		LCollarboneTransform = CharacterMesh->GetSocketTransform(L_COLLARBONE_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		RCollarboneTransform = CharacterMesh->GetSocketTransform(R_COLLARBONE_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);

		WeaponRootAnimTransform = CharacterMesh->GetSocketTransform(WEAPON_ROOT_3RD_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		//WeaponRootAnimTransform = FShooterUtility::TransformToBoneSpace(WeaponMesh, WEAPON_ROOT_SOCKET_NAME, WeaponRootAnimTransform);
	}

}

void UWeaponAnimInstance::AnimNotify_ActionEnd() const
{
	OnWeaponAnimInstanceActionEnd.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_ActionStart() const
{
	OnWeaponAnimInstanceActionStart.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_ChamberCheck() const
{
	OnWeaponAnimInstanceChamberCheck.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_Fire() const
{
	OnWeaponAnimInstanceFire.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_FireDry() const
{
	OnWeaponAnimInstanceFireDry.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_Firemode() const
{
	OnWeaponAnimInstanceFiremode.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_FiremodeCheck() const
{
	OnWeaponAnimInstanceFiremodeCheck.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_Idle() const
{
	OnWeaponAnimInstanceIdle.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_IdleToOut() const
{
	OnWeaponAnimInstanceIdleToOut.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_MagCheck() const
{
	OnWeaponAnimInstanceMagCheck.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_MagIn() const
{
	OnWeaponAnimInstanceMagIn.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_MagOut() const
{
	OnWeaponAnimInstanceMagOut.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_Out() const
{
	OnWeaponAnimInstanceOut.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_OutToIdle() const
{
	OnWeaponAnimInstanceOutToIdle.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_OutToIdleArm() const
{
	OnWeaponAnimInstanceOutToIdleArm.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_ReloadCharge() const
{
	OnWeaponAnimInstanceReloadCharge.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_PatronInWeapon() const
{
	OnWeaponAnimInstancePatronInWeapon.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_WeaponSelector() const
{
	OnWeaponAnimInstanceWeaponSelector.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_WeaponHammer() const
{
	OnWeaponAnimInstanceWeaponHammer.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_ShellPort() const
{
	OnWeaponAnimInstanceShellPort.Broadcast();
}
