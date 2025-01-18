// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/WeaponAnimInstance.h"
#include "Actor/Weapon.h"
#include "Character/ShooterCharacter.h"
#include "Struct/ShooterUtility.h"
#include "Type/ShooterNameType.h"

void UWeaponAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Weapon = Cast<AWeapon>(GetOwningActor());

	if (Weapon)
	{
		CombatAction = Weapon->GetCombatAction();
		FireAnimPlayRate = Weapon->GetRateOfFire() / 60.0f;
		//FireAnimPlayRate = 1.0f;
	}

	IKAlpha = 0.0f;
	IKBlendInOutFlag = 0;
	IKBlendDuration = 0.3f;
	IKBlendDurationCounter = 0.0f;
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
	ForegripHandguardMesh = Weapon->GetForegripHandguardMesh();
	CombatAction = Weapon->GetCombatAction();
	Firemode = Weapon->GetFiremode();
	bIsHolster = Weapon->GetIsHolster();
	bHasForegripHandguardMesh = ForegripHandguardMesh != nullptr;
	bHasPatronInWeaponAmmo = Weapon->HasPatronInWeaponAmmo();
	bIsPistol = Weapon->IsPistol();
	bIsOneHanded = Weapon->GetIsOneHanded();

	AShooterCharacter* ShooterCharacter = Weapon->GetShooterCharacterOwner();
	CharacterMesh = ShooterCharacter == nullptr ? nullptr : ShooterCharacter->GetMesh();
	bHasCharacterVelocity = ShooterCharacter && ShooterCharacter->GetVelocity().SizeSquared2D() > 0.0f;
	bIsCharacterProned = ShooterCharacter && ShooterCharacter->bIsProned;
	bIsCharacterSprinting = ShooterCharacter && ShooterCharacter->bIsSprinting;
	bIsCharacterThirdAction = ShooterCharacter && ShooterCharacter->IsThirdAction();

	if (bIsCharacterThirdAction)
	{
		IKAlpha = 1.0f;
	}
	else
	{
		if (bHasForegripHandguardMesh)
		{
			IKBlendDurationCounter += IKBlendInOutFlag * DeltaSeconds;
			IKBlendDurationCounter = FMath::Clamp(IKBlendDurationCounter, 0.0f, IKBlendDuration);
			IKAlpha = IKBlendDurationCounter / IKBlendDuration;
		}
		else
		{
			IKAlpha = 0.0f;
		}
	}

	if (ForegripHandguardMesh)
	{
		LPalmTransform = ForegripHandguardMesh->GetSocketTransform(BASE_HUMAN_L_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
	}
	if (CharacterMesh && bIsCharacterThirdAction)
	{
		BendGoalLeftTransform = CharacterMesh->GetSocketTransform(BEND_GOAL_LEFT_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		BendGoalRightTransform = CharacterMesh->GetSocketTransform(BEND_GOAL_RIGHT_SOCKET_NAME, ERelativeTransformSpace::RTS_World);

		LCollarboneTransform = CharacterMesh->GetSocketTransform(L_COLLARBONE_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		RCollarboneTransform = CharacterMesh->GetSocketTransform(R_COLLARBONE_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);

		if ((bIsCharacterProned && bHasCharacterVelocity) || (bIsCharacterSprinting && (bIsPistol || bIsOneHanded)))
		{
			LPalmTransform = CharacterMesh->GetSocketTransform(IK_S_L_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
			RPalmTransform = CharacterMesh->GetSocketTransform(IK_S_R_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		}

		WeaponRootAnimTransform = CharacterMesh->GetSocketTransform(WEAPON_ROOT_3RD_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
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

void UWeaponAnimInstance::AnimNotify_WeaponLHandMarker()
{
	IKBlendInOutFlag = 1;
}

void UWeaponAnimInstance::AnimNotify_WeaponLIKMarker()
{
	IKBlendInOutFlag = -1;
}
