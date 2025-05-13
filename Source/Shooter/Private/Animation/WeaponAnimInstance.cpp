// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/WeaponAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/ShooterCharacter.h"
#include "Components/CharacterCombatComponent.h"
#include "Mod/Foregrip.h"
#include "Mod/Handguard.h"
#include "Types/ShooterNames.h"
#include "Utility/ShooterUtility.h"
#include "Weapon/Weapon.h"

void UWeaponAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Weapon = Cast<AWeapon>(GetOwningActor());
	WeaponMesh = GetOwningComponent();

	if (Weapon)
	{
		FireAnimPlayRate = Weapon->GetRateOfFire() / 60.0f;
		//FireAnimPlayRate = 1.0f;
	}

	CombatAction = ECombatAction::CA_Out;

	IKAlpha = 0.0f;
	IKBlendInOutFlag = 0;
	IKBlendDuration = 0.3f;
	IKBlendDurationCounter = 0.0f;

	SwayInterpSpeed = 5.0f;
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

	Firemode = Weapon->GetFiremode();
	bIsHolster = Weapon->GetIsHolster();
	bIsPistol = Weapon->IsPistol();
	bIsOneHanded = Weapon->GetIsOneHanded();
	bHasPatronInWeaponAmmo = Weapon->GetPatronInWeaponAmmo() != nullptr;

	AMod* AttachedForegrip = Weapon->GetAttachedActor<AForegrip>();
	AMod* AttachedHandguard = Weapon->GetAttachedActor<AHandguard>();
	ForegripHandguardMesh = AttachedForegrip ? AttachedForegrip->GetMesh() : AttachedHandguard ? AttachedHandguard->GetMesh() : nullptr;
	bHasForegripHandguardMesh = ForegripHandguardMesh != nullptr;

	ShooterCharacter = Weapon->GetShooterCharacterOwner();
	CharacterMesh = ShooterCharacter != nullptr ? ShooterCharacter->GetMesh() : nullptr;
	bIsCharacterProned = ShooterCharacter && ShooterCharacter->bIsProned;
	bIsCharacterSprinting = ShooterCharacter && ShooterCharacter->bIsSprinting;
	bHasCharacterVelocity = ShooterCharacter && ShooterCharacter->GetVelocity().SizeSquared2D() > 0.0f;

	bool bIsCharacterTransition = ShooterCharacter && ShooterCharacter->GetIsTransition();
	bIsCharacterThirdAction = bIsCharacterTransition || bIsCharacterSprinting || (bIsCharacterProned && bHasCharacterVelocity);

	AWeapon* CharacterEquippedWeapon = ShooterCharacter != nullptr ? ShooterCharacter->GetEquippedWeapon() : nullptr;
	ECombatAction CharacterCombatAction = ShooterCharacter != nullptr ? ShooterCharacter->GetCombatAction() : ECombatAction::CA_None;
	CombatAction = Weapon == CharacterEquippedWeapon ? CharacterCombatAction : ECombatAction::CA_Out;

	AO_Pitch = ShooterCharacter != nullptr ? ShooterCharacter->GetAO_Pitch() : 0.0f;
	AO_Yaw = ShooterCharacter != nullptr ? ShooterCharacter->GetAO_Yaw() : 0.0f;

	ControlRotationLast = ControlRotation;
	ControlRotation = ShooterCharacter != nullptr ? ShooterCharacter->GetControlRotation() : FRotator::ZeroRotator;

	CalculateIKAlpha(DeltaSeconds);

	CalculateSway(DeltaSeconds);

	CalculateTransforms();

	// For later use
	/*if (bHasCharacterVelocity)
	{
		float CharacterVelocityYawOffset = CalculateCharacterVelocityYawOffset();
		float CharacterVelocityYawOffsetAlpha = CalculateVelocityYawOffsetAlpha(CharacterVelocityYawOffset);
		float WalkAnimMaxHorizontalMovementOffset;
		float WalkAnimMinHorizontalMovementOffset;
		if (CharacterVelocityYawOffsetAlpha > 0.0f)
		{
			WalkAnimMaxHorizontalMovementOffset = FMath::FInterpTo(WalkAnimMaxHorizontalMovementOffset, WalkAnimMaxHorizontalMovement * CharacterVelocityYawOffsetAlpha, DeltaSeconds, 5.0f);
			WalkAnimMinHorizontalMovementOffset = FMath::FInterpTo(WalkAnimMinHorizontalMovementOffset, 0, DeltaSeconds, 5.0f);
		}
		else if (CharacterVelocityYawOffsetAlpha < 0.0f)
		{
			WalkAnimMaxHorizontalMovementOffset = FMath::FInterpTo(WalkAnimMaxHorizontalMovementOffset, 0, DeltaSeconds, 5.0f);
			WalkAnimMinHorizontalMovementOffset = FMath::FInterpTo(WalkAnimMinHorizontalMovementOffset, WalkAnimMinHorizontalMovement * CharacterVelocityYawOffsetAlpha, DeltaSeconds, 5.0f);
		}
	}*/

}

void UWeaponAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (Weapon == nullptr || ShooterCharacter == nullptr)
	{
		return;
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

void UWeaponAnimInstance::CalculateIKAlpha(float DeltaSeconds)
{
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
			IKAlpha = CombatAction == ECombatAction::CA_Idle ? 1.0f : 0.0f;
		}
	}
}

void UWeaponAnimInstance::CalculateSway(float DeltaSeconds)
{
	FRotator DeltaControlRotation = UKismetMathLibrary::NormalizedDeltaRotator(ControlRotation, ControlRotationLast);
	float SwayHorizontalMovementLimit = ShooterCharacter != nullptr ? ShooterCharacter->GetSwayHorizontalMovementLimit() : 0.0f;
	float SwayVerticalMovementLimit = ShooterCharacter != nullptr ? ShooterCharacter->GetSwayVerticalMovementLimit() : 0.0f;
	float SwayRollRotationLimit = ShooterCharacter != nullptr ? ShooterCharacter->GetSwayRollRotationLimit() : 0.0f;
	float SwayHorizontalMovementSensitivity = ShooterCharacter != nullptr ? ShooterCharacter->GetSwayHorizontalMovementSensitivity() : 0.0f;
	float SwayVerticalMovementSensitivity = ShooterCharacter != nullptr ? ShooterCharacter->GetSwayVerticalMovementSensitivity() : 0.0f;
	float SwayRollRotationSensitivity = ShooterCharacter != nullptr ? ShooterCharacter->GetSwayRollRotationSensitivity() : 0.0f;

	SwayHorizontalMovement = FMath::FInterpTo(SwayHorizontalMovement, 0.0f, DeltaSeconds, SwayInterpSpeed);
	SwayHorizontalMovement += DeltaControlRotation.Yaw * SwayHorizontalMovementSensitivity;
	SwayHorizontalMovement = FMath::Clamp(SwayHorizontalMovement, -SwayHorizontalMovementLimit, SwayHorizontalMovementLimit);

	SwayVerticalMovement = FMath::FInterpTo(SwayVerticalMovement, 0.0f, DeltaSeconds, SwayInterpSpeed);
	SwayVerticalMovement += DeltaControlRotation.Pitch * SwayVerticalMovementSensitivity;
	SwayVerticalMovement = FMath::Clamp(SwayVerticalMovement, -SwayVerticalMovementLimit, SwayVerticalMovementLimit);

	SwayRollRotation = FMath::FInterpTo(SwayRollRotation, 0.0f, DeltaSeconds, SwayInterpSpeed);
	SwayRollRotation += DeltaControlRotation.Yaw * SwayRollRotationSensitivity;
	SwayRollRotation = FMath::Clamp(SwayRollRotation, -SwayRollRotationLimit, SwayRollRotationLimit);
}

void UWeaponAnimInstance::CalculateTransforms()
{
	CalculateRootBoneLocation();

	if (!ShouldCopyCharacterIKSLPalm())
	{
		CalculateLPalmTransform();
	}

	if (bIsCharacterThirdAction)
	{
		CalculateThirdActionTransforms();
	}
	else
	{
		CalculateWeaponRootAnimTransform();
	}
}

void UWeaponAnimInstance::CalculateRootBoneLocation()
{
	if (CharacterMesh)
	{
		FTransform CharacterRibcageTransform = CharacterMesh->GetSocketTransform(BASE_HUMAN_RIBCAGE_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		RootBoneLocation = FShooterUtility::TransformToBoneSpace(CharacterMesh, ROOT_JOINT_SOCKET_NAME, CharacterRibcageTransform).GetLocation();
	}
	else
	{
		RootBoneLocation = FVector::ZeroVector;
	}
}

void UWeaponAnimInstance::CalculateLPalmTransform()
{
	if (WeaponMesh && ForegripHandguardMesh)
	{
		LPalmTransform = ForegripHandguardMesh->GetSocketTransform(BASE_HUMAN_L_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		LPalmTransform = FShooterUtility::TransformToBoneSpace(WeaponMesh, WEAPON_SOCKET_NAME, LPalmTransform);
	}
}

void UWeaponAnimInstance::CalculateThirdActionTransforms()
{
	if (CharacterMesh && WeaponMesh)
	{
		/*LCollarboneTransform = CharacterMesh->GetSocketTransform(L_COLLARBONE_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		LCollarboneTransform = FShooterUtility::TransformToBoneSpace(WeaponMesh, WeaponMesh->GetParentBone(BASE_HUMAN_L_COLLARBONE_SOCKET_NAME), LCollarboneTransform);*/

		/*RCollarboneTransform = CharacterMesh->GetSocketTransform(R_COLLARBONE_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		RCollarboneTransform = FShooterUtility::TransformToBoneSpace(WeaponMesh, WeaponMesh->GetParentBone(BASE_HUMAN_R_COLLARBONE_SOCKET_NAME), RCollarboneTransform);*/

		WeaponRootAnimTransform = CharacterMesh->GetSocketTransform(WEAPON_ROOT_3RD_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		WeaponRootAnimTransform = FShooterUtility::TransformToBoneSpace(WeaponMesh, WEAPON_ROOT_SOCKET_NAME, WeaponRootAnimTransform);

		BendGoalLeftTransform = CharacterMesh->GetSocketTransform(BEND_GOAL_LEFT_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		BendGoalLeftTransform = FShooterUtility::TransformToBoneSpace(WeaponMesh, WEAPON_SOCKET_NAME, BendGoalLeftTransform);

		BendGoalRightTransform = CharacterMesh->GetSocketTransform(BEND_GOAL_RIGHT_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		BendGoalRightTransform = FShooterUtility::TransformToBoneSpace(WeaponMesh, WEAPON_SOCKET_NAME, BendGoalRightTransform);

		if (ShouldCopyCharacterIKSLPalm())
		{
			LPalmTransform = CharacterMesh->GetSocketTransform(IK_S_L_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
			LPalmTransform = FShooterUtility::TransformToBoneSpace(WeaponMesh, WEAPON_SOCKET_NAME, LPalmTransform);
		}

		/*RPalmTransform = CharacterMesh->GetSocketTransform(IK_S_R_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		RPalmTransform = FShooterUtility::TransformToBoneSpace(WeaponMesh, WEAPON_SOCKET_NAME, RPalmTransform);*/
	}
}

void UWeaponAnimInstance::CalculateWeaponRootAnimTransform()
{
	float ProceduralAnimHorizontalMovement = ShooterCharacter != nullptr ? ShooterCharacter->GetProceduralAnimHorizontalMovement() : 0.0f;
	float ProceduralAnimVerticalMovement = ShooterCharacter != nullptr ? ShooterCharacter->GetProceduralAnimVerticalMovement() : 0.0f;
	float ProceduralAnimRollRotation = ShooterCharacter != nullptr ? ShooterCharacter->GetProceduralAnimRollRotation() : 0.0f;

	WeaponRootAnimTransform = FTransform(FRotator(ProceduralAnimRollRotation + SwayRollRotation, 0.0f, 0.0f), FVector(ProceduralAnimHorizontalMovement + SwayHorizontalMovement, 0.0f, ProceduralAnimVerticalMovement + SwayVerticalMovement), FVector::OneVector);
}

bool UWeaponAnimInstance::ShouldCopyCharacterIKSLPalm()
{
	return (bIsCharacterProned && bHasCharacterVelocity) || (bIsCharacterSprinting && (bIsPistol || bIsOneHanded));
}

//float UWeaponAnimInstance::CalculateVelocityYawOffsetAlpha(float VelocityYawOffset)
//{
//	float CharacterVelocityYawOffsetAlpha = UKismetMathLibrary::NormalizeToRange(VelocityYawOffset, 0.0f, 90.0f);
//	if (FMath::Abs(CharacterVelocityYawOffsetAlpha) > 1.0f)
//	{
//		FVector2D InRange = FVector2D(1.0f, 2.0f);
//		FVector2D OutRange = FVector2D(1.0, 0.0);
//		if (CharacterVelocityYawOffsetAlpha < 0.0f)
//		{
//			InRange *= -1;
//			OutRange *= -1;
//		}
//		CharacterVelocityYawOffsetAlpha = FMath::GetMappedRangeValueClamped(InRange, OutRange, CharacterVelocityYawOffsetAlpha);
//	}
//	return CharacterVelocityYawOffsetAlpha;
//}
