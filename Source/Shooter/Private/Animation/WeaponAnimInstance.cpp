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

	ShooterCharacter = Weapon->GetShooterCharacterOwner();
	if (ShooterCharacter == nullptr)
	{
		return;
	}
	
	//float CharacterSwayHorizontalMovementLimit = ShooterCharacter->GetSwayHorizontalMovementLimit();
	//float CharacterSwayVerticalMovementLimit = ShooterCharacter->GetSwayVerticalMovementLimit();
	//float CharacterSwayRollRotationLimit = ShooterCharacter->GetSwayRollRotationLimit();
	//float CharacterSwayHorizontalMovementSensitivity = ShooterCharacter->GetSwayHorizontalMovementSensitivity();
	//float CharacterSwayVerticalMovementSensitivity = ShooterCharacter->GetSwayVerticalMovementSensitivity();
	//float CharacterSwayRollRotationSensitivity = ShooterCharacter->GetSwayRollRotationSensitivity();

	Firemode = Weapon->GetFiremode();
	bIsHolster = Weapon->GetIsHolster();
	bIsPistol = Weapon->IsPistol();
	bIsOneHanded = Weapon->GetIsOneHanded();
	bHasPatronInWeaponAmmo = Weapon->GetPatronInWeaponAmmo() != nullptr;

	AMod* AttachedForegrip = Weapon->GetAttachedActor<AForegrip>();
	AMod* AttachedHandguard = Weapon->GetAttachedActor<AHandguard>();
	ForegripHandguardMesh = AttachedForegrip ? AttachedForegrip->GetMesh() : AttachedHandguard ? AttachedHandguard->GetMesh() : nullptr;
	bHasForegripHandguardMesh = ForegripHandguardMesh != nullptr;

	//CharacterBaseAimRotationLast = CharacterBaseAimRotation;
	//CharacterBaseAimRotation = ShooterCharacter->GetBaseAimRotation();

	CharacterMesh = ShooterCharacter->GetMesh();
	bIsCharacterProned = ShooterCharacter->bIsProned;
	bIsCharacterSprinting = ShooterCharacter->bIsSprinting;
	ProceduralAnimHorizontalMovement = ShooterCharacter->GetProceduralAnimHorizontalMovement();
	ProceduralAnimVerticalMovement = ShooterCharacter->GetProceduralAnimVerticalMovement();
	ProceduralAnimRollRotation = ShooterCharacter->GetProceduralAnimRollRotation();

	bHasCharacterVelocity = ShooterCharacter->GetVelocity().SizeSquared2D() > 0.0f;

	bool bIsCharacterTransition = ShooterCharacter->GetIsTransition();
	bIsCharacterThirdAction = bIsCharacterTransition || bIsCharacterSprinting || (bIsCharacterProned && bHasCharacterVelocity);

	AWeapon* CharacterEquippedWeapon = ShooterCharacter->GetEquippedWeapon();
	ECombatAction CharacterCombatAction = ShooterCharacter->GetCombatAction();
	CombatAction = Weapon == CharacterEquippedWeapon ? CharacterCombatAction : ECombatAction::CA_Out;

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

	float InterpSpeed = 5.0f;
	SwayHorizontalMovement = FMath::FInterpTo(SwayHorizontalMovement, 0.0f, DeltaSeconds, InterpSpeed);
	SwayVerticalMovement = FMath::FInterpTo(SwayVerticalMovement, 0.0f, DeltaSeconds, InterpSpeed);
	SwayRollRotation = FMath::FInterpTo(SwayRollRotation, 0.0f, DeltaSeconds, InterpSpeed);

	CharacterControlRotationLast = CharacterControlRotation;
	CharacterControlRotation = ShooterCharacter->GetControlRotation();
	FRotator DeltaCharacterControlRotation = UKismetMathLibrary::NormalizedDeltaRotator(CharacterControlRotation, CharacterControlRotationLast);

	float CharacterSwayHorizontalMovementLimit = ShooterCharacter->GetSwayHorizontalMovementLimit();
	SwayHorizontalMovement += DeltaCharacterControlRotation.Yaw * ShooterCharacter->GetSwayHorizontalMovementSensitivity();
	SwayHorizontalMovement = FMath::Clamp(SwayHorizontalMovement, -CharacterSwayHorizontalMovementLimit, CharacterSwayHorizontalMovementLimit);

	float CharacterSwayVerticalMovementLimit = ShooterCharacter->GetSwayVerticalMovementLimit();
	SwayVerticalMovement += DeltaCharacterControlRotation.Pitch * ShooterCharacter->GetSwayVerticalMovementSensitivity();
	SwayVerticalMovement = FMath::Clamp(SwayVerticalMovement, -CharacterSwayVerticalMovementLimit, CharacterSwayVerticalMovementLimit);

	float CharacterSwayRollRotationLimit = ShooterCharacter->GetSwayRollRotationLimit();
	SwayRollRotation += DeltaCharacterControlRotation.Yaw * ShooterCharacter->GetSwayRollRotationSensitivity();
	SwayRollRotation = FMath::Clamp(SwayRollRotation, -CharacterSwayRollRotationLimit, CharacterSwayRollRotationLimit);

	// Old
	/*if (ForegripHandguardMesh)
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
	}*/
	// New
	if (WeaponMesh)
	{
		FName RootBoneName = WeaponMesh->GetBoneName(0);
		if (ForegripHandguardMesh)
		{
			LPalmTransform = ForegripHandguardMesh->GetSocketTransform(BASE_HUMAN_L_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
			LPalmTransform = FShooterUtility::TransformToBoneSpace(WeaponMesh, RootBoneName, LPalmTransform);
		}
		if (CharacterMesh && bIsCharacterThirdAction)
		{
			BendGoalLeftTransform = CharacterMesh->GetSocketTransform(BEND_GOAL_LEFT_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
			BendGoalLeftTransform = FShooterUtility::TransformToBoneSpace(WeaponMesh, RootBoneName, BendGoalLeftTransform);

			BendGoalRightTransform = CharacterMesh->GetSocketTransform(BEND_GOAL_RIGHT_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
			BendGoalRightTransform = FShooterUtility::TransformToBoneSpace(WeaponMesh, RootBoneName, BendGoalRightTransform);

			LCollarboneTransform = CharacterMesh->GetSocketTransform(L_COLLARBONE_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
			LCollarboneTransform = FShooterUtility::TransformToBoneSpace(WeaponMesh, RootBoneName, LCollarboneTransform);

			RCollarboneTransform = CharacterMesh->GetSocketTransform(R_COLLARBONE_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
			RCollarboneTransform = FShooterUtility::TransformToBoneSpace(WeaponMesh, RootBoneName, RCollarboneTransform);

			if ((bIsCharacterProned && bHasCharacterVelocity) || (bIsCharacterSprinting && (bIsPistol || bIsOneHanded)))
			{
				LPalmTransform = CharacterMesh->GetSocketTransform(IK_S_L_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
				LPalmTransform = FShooterUtility::TransformToBoneSpace(WeaponMesh, RootBoneName, LPalmTransform);

				RPalmTransform = CharacterMesh->GetSocketTransform(IK_S_R_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
				RPalmTransform = FShooterUtility::TransformToBoneSpace(WeaponMesh, RootBoneName, RPalmTransform);
			}

			WeaponRootAnimTransform = CharacterMesh->GetSocketTransform(WEAPON_ROOT_3RD_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
			WeaponRootAnimTransform = FShooterUtility::TransformToBoneSpace(WeaponMesh, RootBoneName, WeaponRootAnimTransform);
		}
	}

	// Disable for now until new transform logic is set
	/*if (!bIsHolster && CombatAction == ECombatAction::CA_Idle && !bIsCharacterThirdAction)
	{
		FVector ProceduralAnimLocation;
		FRotator ProceduralAnimRotation;
		ProceduralAnimLocation.X = ProceduralAnimHorizontalMovement + SwayHorizontalMovement;
		ProceduralAnimLocation.Z = ProceduralAnimVerticalMovement - SwayVerticalMovement;
		ProceduralAnimRotation.Pitch = ProceduralAnimRollRotation + SwayRollRotation;
		WeaponRootAnimTransform.SetLocation(ProceduralAnimLocation);
		WeaponRootAnimTransform.SetRotation(FQuat(ProceduralAnimRotation));
	}*/

	//CharacterMesh = ShooterCharacter != nullptr ? ShooterCharacter->GetMesh() : nullptr;
	//bIsCharacterProned = ShooterCharacter && ShooterCharacter->bIsProned;
	//bIsCharacterSprinting = ShooterCharacter && ShooterCharacter->bIsSprinting;
	//bHasCharacterVelocity = ShooterCharacter && ShooterCharacter->GetVelocity().SizeSquared2D() > 0.0f;
	//bIsCharacterThirdAction = (ShooterCharacter && ShooterCharacter->GetIsTransition()) || bIsCharacterSprinting || (bIsCharacterProned && bHasCharacterVelocity);

	//ProceduralAnimHorizontalMovement = ShooterCharacter != nullptr ? ShooterCharacter->GetProceduralAnimHorizontalMovement() : 0.0f;
	//ProceduralAnimVerticalMovement = ShooterCharacter != nullptr ? ShooterCharacter->GetProceduralAnimVerticalMovement() : 0.0f;
	//ProceduralAnimRollRotation = ShooterCharacter != nullptr ? ShooterCharacter->GetProceduralAnimRollRotation() : 0.0f;

	//CharacterViewRotationLast = CharacterViewRotation;
	//CharacterViewRotation = ShooterCharacter != nullptr ? ShooterCharacter->GetBaseAimRotation() : FRotator::ZeroRotator;
	
	/*UCharacterCombatComponent* CharacterCombatComponent = ShooterCharacter != nullptr ? ShooterCharacter->GetCharacterCombatComponent() : nullptr;
	AWeapon* CharacterEquippedWeapon = CharacterCombatComponent != nullptr ? CharacterCombatComponent->GetEquippedWeapon() : nullptr;
	CombatAction = CharacterCombatComponent && Weapon == CharacterEquippedWeapon ? CharacterCombatComponent->GetCombatAction() : ECombatAction::CA_Out;*/
	/*AWeapon* EquippedWeapon = ShooterCharacter != nullptr ? ShooterCharacter->GetEquippedWeapon() : nullptr;
	CombatAction = ShooterCharacter && ShooterCharacter->GetEquippedWeapon() == Weapon ? ShooterCharacter->GetCombatAction() : ECombatAction::CA_Out;*/

	

	// In ThreadSafe?
	/*InterpBackSway(DeltaSeconds, 5.0f);
	CalculateSway(UKismetMathLibrary::NormalizedDeltaRotator(CharacterViewRotation, CharacterViewRotationLast));*/

	// In ThreadSafe?
	/*if (ForegripHandguardMesh)
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
	}*/

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

	// In ThreadSafe?
	/*if (!bIsHolster && CombatAction == ECombatAction::CA_Idle && !bIsCharacterThirdAction)
	{
		FVector ProceduralAnimLocation;
		FRotator ProceduralAnimRotation;
		ProceduralAnimLocation.X = ProceduralAnimHorizontalMovement + SwayHorizontalMovement;
		ProceduralAnimLocation.Z = ProceduralAnimVerticalMovement - SwayVerticalMovement;
		ProceduralAnimRotation.Pitch = ProceduralAnimRollRotation + SwayRollRotation;
		WeaponRootAnimTransform.SetLocation(ProceduralAnimLocation);
		WeaponRootAnimTransform.SetRotation(FQuat(ProceduralAnimRotation));
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

//void UWeaponAnimInstance::CalculateSway(FRotator DeltaRotation)
//{
//	if (ShooterCharacter)
//	{
//		ShooterCharacter->CalculateSwayHorizontalMovement(SwayHorizontalMovement, DeltaRotation.Yaw);
//		ShooterCharacter->CalculateSwayVerticalMovement(SwayVerticalMovement, DeltaRotation.Pitch);
//		ShooterCharacter->CalculateSwayRollRotation(SwayRollRotation, DeltaRotation.Yaw);
//	}
//}

//void UWeaponAnimInstance::InterpBackSway(float DeltaSeconds, float InterpSpeed)
//{
//	SwayHorizontalMovement = FMath::FInterpTo(SwayHorizontalMovement, 0.0f, DeltaSeconds, InterpSpeed);
//	SwayVerticalMovement = FMath::FInterpTo(SwayVerticalMovement, 0.0f, DeltaSeconds, InterpSpeed);
//	SwayRollRotation = FMath::FInterpTo(SwayRollRotation, 0.0f, DeltaSeconds, InterpSpeed);
//}

//void UWeaponAnimInstance::CalculateIKAlpha(float DeltaSeconds)
//{
//	if (bIsCharacterThirdAction)
//	{
//		IKAlpha = 1.0f;
//	}
//	else
//	{
//		if (bHasForegripHandguardMesh)
//		{
//			IKBlendDurationCounter += IKBlendInOutFlag * DeltaSeconds;
//			IKBlendDurationCounter = FMath::Clamp(IKBlendDurationCounter, 0.0f, IKBlendDuration);
//			IKAlpha = IKBlendDurationCounter / IKBlendDuration;
//		}
//		else
//		{
//			IKAlpha = CombatAction == ECombatAction::CA_Idle ? 1.0f : 0.0f;
//		}
//	}
//}
