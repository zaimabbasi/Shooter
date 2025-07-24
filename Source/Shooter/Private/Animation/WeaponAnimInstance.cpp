// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/WeaponAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/ShooterCharacter.h"
#include "Types/CombatTypes.h"
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

		RecoilKickTotalTime = (1.0f / FireAnimPlayRate) / 2.0f;
	}

	OnLHandInUpdate.BindUFunction(this, TEXT("Handle_OnLHandInUpdate"));
	OnLHandOutUpdate.BindUFunction(this, TEXT("Handle_OnLHandOutUpdate"));

	IKBlendDuration = 0.3f;

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
	bIsArmed = Weapon->GetIsArmed();
	//bIsHolster = Weapon->GetIsHolster();
	bIsPistol = Weapon->IsPistol();
	bIsOneHanded = Weapon->GetIsOneHanded();
	bIsBoltCatched = Weapon->GetIsBoltCatched();
	ForegripHandguardMesh = Weapon->GetForegripHandguardMesh();
	bHasForegripHandguardMesh = ForegripHandguardMesh != nullptr;

	ShooterCharacter = Weapon->GetShooterCharacterOwner();
	CharacterMesh = ShooterCharacter != nullptr ? ShooterCharacter->GetMesh() : nullptr;
	bIsCharacterProned = ShooterCharacter && ShooterCharacter->bIsProned;
	bIsCharacterSprinting = ShooterCharacter && ShooterCharacter->bIsSprinting;
	bHasCharacterVelocity = ShooterCharacter && ShooterCharacter->GetVelocity().SizeSquared2D() > 0.0f;

	bool bIsCharacterTransition = ShooterCharacter && ShooterCharacter->GetIsTransition();
	bIsCharacterThirdActionLast = bIsCharacterThirdAction;
	bIsCharacterThirdAction = bIsCharacterTransition || bIsCharacterSprinting || (bIsCharacterProned && bHasCharacterVelocity);

	AWeapon* CharacterEquippedWeapon = ShooterCharacter != nullptr ? ShooterCharacter->GetEquippedWeapon() : nullptr;
	ECombatAction CharacterCombatAction = ShooterCharacter != nullptr ? ShooterCharacter->GetCombatAction() : ECombatAction::CA_None;
	CombatAction = Weapon == CharacterEquippedWeapon ? CharacterCombatAction : ECombatAction::CA_Out;

	AO_Pitch = ShooterCharacter != nullptr ? ShooterCharacter->GetAO_Pitch() : 0.0f;
	AO_Yaw = ShooterCharacter != nullptr ? ShooterCharacter->GetAO_Yaw() : 0.0f;

	ControlRotationLast = ControlRotation;
	ControlRotation = ShooterCharacter != nullptr ? ShooterCharacter->GetControlRotation() : FRotator::ZeroRotator;

	RecoilRecoveryTotalTime = ShooterCharacter != nullptr ? ShooterCharacter->GetRecoilRecoveryTotalTime() / 2.0f : 1.0f;

	CalculateLHandMarkerAlpha();

	CalculateSway(DeltaSeconds);

	CalculateRecoil(DeltaSeconds);

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

void UWeaponAnimInstance::SetCombatAction(ECombatAction NewCombatAction)
{
	CombatAction = NewCombatAction;
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
	if (CombatAction == ECombatAction::CA_Fire)
	{
		OnWeaponAnimInstanceFire.Broadcast();
	}
}

void UWeaponAnimInstance::AnimNotify_FireDry()
{
	SnapshotPose(PoseSnapshot);

	OnWeaponAnimInstanceFireDry.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_Firemode()
{
	SnapshotPose(PoseSnapshot);

	OnWeaponAnimInstanceFiremode.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_FiremodeCheck() const
{
	OnWeaponAnimInstanceFiremodeCheck.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_HandsInHalf1() const
{
	OnWeaponAnimInstanceHandsInHalf1.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_HandsInHalf2() const
{
	OnWeaponAnimInstanceHandsInHalf2.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_HandsOutHalf1() const
{
	OnWeaponAnimInstanceHandsOutHalf1.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_HandsOutHalf2() const
{
	OnWeaponAnimInstanceHandsOutHalf2.Broadcast();
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

void UWeaponAnimInstance::AnimNotify_OutToIdleArm()
{
	SnapshotPose(PoseSnapshot);

	OnWeaponAnimInstanceOutToIdleArm.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_ReloadCatch()
{
	SnapshotPose(PoseSnapshot);

	OnWeaponAnimInstanceReloadCatch.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_ReloadCharge()
{
	SnapshotPose(PoseSnapshot);

	OnWeaponAnimInstanceReloadCharge.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_BoltCatch()
{
	if (CombatAction == ECombatAction::CA_Fire)
	{
		OnWeaponAnimInstanceBoltCatch.Broadcast();

		if (Weapon && Weapon->GetIsBoltCatched())
		{
			SnapshotPose(PoseSnapshot);
		}
	}
}

void UWeaponAnimInstance::AnimNotify_IdleStart() const
{
	OnWeaponAnimInstanceIdleStart.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_PatronInWeapon() const
{
	if (CombatAction == ECombatAction::CA_Fire || CombatAction == ECombatAction::CA_OutToIdleArm || CombatAction == ECombatAction::CA_ReloadCatch || CombatAction == ECombatAction::CA_ReloadCharge)
	{
		OnWeaponAnimInstancePatronInWeapon.Broadcast();
	}
}

void UWeaponAnimInstance::AnimNotify_WeaponSelector()
{
	SnapshotPose(PoseSnapshot);

	OnWeaponAnimInstanceWeaponSelector.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_WeaponHammer()
{
	if (CombatAction == ECombatAction::CA_Fire)
	{
		bIsRecoilKick = true;
		bIsRecoilRecovery = false;

		OnWeaponAnimInstanceWeaponHammer.Broadcast();
	}
}

void UWeaponAnimInstance::AnimNotify_ShellPort() const
{
	if (CombatAction == ECombatAction::CA_Fire)
	{
		OnWeaponAnimInstanceShellPort.Broadcast();
	}
}

void UWeaponAnimInstance::AnimNotify_SoundDryFire() const
{
	OnWeaponAnimInstanceSoundDryFire.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_LHandIn()
{
	if (OnLHandOutUpdateHandle.Pin())
	{
		StopLHandOutUpdate();
	}
	StartLHandInUpdate();
}

void UWeaponAnimInstance::AnimNotify_LHandOut()
{
	if (OnLHandInUpdateHandle.Pin())
	{
		StopLHandInUpdate();
	}
	StartLHandOutUpdate();
}

bool UWeaponAnimInstance::Handle_OnLHandInUpdate(float DeltaSeconds)
{
	float WeaponLHandMarkerAlphaRemaining = 1.0f - WeaponLHandMarkerAlpha;
	float WeaponLHandMarkerAlphaStep = (1.0f / IKBlendDuration) * DeltaSeconds;
	float WeaponLHandMarkerAlphaToAdd = WeaponLHandMarkerAlphaStep > WeaponLHandMarkerAlphaRemaining ? WeaponLHandMarkerAlphaRemaining : WeaponLHandMarkerAlphaStep;

	WeaponLHandMarkerAlpha += WeaponLHandMarkerAlphaToAdd;

	if (WeaponLHandMarkerAlpha == 1.0f)
	{
		StopLHandInUpdate();

		return false;
	}

	return true;
}

bool UWeaponAnimInstance::Handle_OnLHandOutUpdate(float DeltaSeconds)
{
	float WeaponLHandMarkerAlphaRemaining = 0.0f - WeaponLHandMarkerAlpha;
	float WeaponLHandMarkerAlphaStep = (1.0f / IKBlendDuration) * DeltaSeconds * -1.0f;
	float WeaponLHandMarkerAlphaToAdd = FMath::Abs(WeaponLHandMarkerAlphaStep) > FMath::Abs(WeaponLHandMarkerAlphaRemaining) ? WeaponLHandMarkerAlphaRemaining : WeaponLHandMarkerAlphaStep;

	WeaponLHandMarkerAlpha += WeaponLHandMarkerAlphaToAdd;

	if (WeaponLHandMarkerAlpha == 0.0f)
	{
		StartLHandOutUpdate();

		return false;
	}

	return true;
}

void UWeaponAnimInstance::StartLHandInUpdate()
{
	if (!OnLHandInUpdateHandle.Pin())
	{
		OnLHandInUpdateHandle = FTSTicker::GetCoreTicker().AddTicker(OnLHandInUpdate);
	}
}

void UWeaponAnimInstance::StopLHandInUpdate()
{
	FTSTicker::GetCoreTicker().RemoveTicker(OnLHandInUpdateHandle);
}

void UWeaponAnimInstance::StartLHandOutUpdate()
{
	if (!OnLHandOutUpdateHandle.Pin())
	{
		OnLHandOutUpdateHandle = FTSTicker::GetCoreTicker().AddTicker(OnLHandOutUpdate);
	}
}

void UWeaponAnimInstance::StopLHandOutUpdate()
{
	FTSTicker::GetCoreTicker().RemoveTicker(OnLHandOutUpdateHandle);
}

bool UWeaponAnimInstance::ShouldCopyCharacterIKSLPalm() const
{
	return (bIsCharacterProned && bHasCharacterVelocity) || (bIsCharacterSprinting && (bIsPistol || bIsOneHanded));
}

void UWeaponAnimInstance::CalculateLHandMarkerAlpha()
{
	if (bIsCharacterThirdAction && !bIsCharacterThirdActionLast)
	{
		WeaponLHandMarkerAlphaLast = WeaponLHandMarkerAlpha;
		WeaponLHandMarkerAlpha = 1.0f;
	}
	else if (!bIsCharacterThirdAction && bIsCharacterThirdActionLast)
	{
		WeaponLHandMarkerAlpha = WeaponLHandMarkerAlphaLast;
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

void UWeaponAnimInstance::CalculateRecoil(float DeltaSeconds)
{
	if (bIsRecoilKick)
	{
		float RecoilKickMaxDistance = Weapon != nullptr ? Weapon->GetRecoilKickMaxDistance() : 0.0f;
		float RecoilKickMaxPitchRotation = Weapon != nullptr ? Weapon->GetRecoilKickMaxPitchRotation() : 0.0f;

		float RecoilKickMovementRemaining = RecoilKickMaxDistance - RecoilKickMovement;
		float RecoilKickPitchRotationRemainig = RecoilKickMaxPitchRotation - RecoilKickPitchRotation;

		float RecoilKickMovementStep = (RecoilKickMaxDistance / RecoilKickTotalTime) * DeltaSeconds;
		float RecoilKickPitchRotationStep = (RecoilKickMaxPitchRotation / RecoilKickTotalTime) * DeltaSeconds;

		float RecoilKickMovmentToAdd = RecoilKickMovementStep > RecoilKickMovementRemaining ? RecoilKickMovementRemaining : RecoilKickMovementStep;
		float RecoilPitchRotationToAdd = RecoilKickPitchRotationStep > RecoilKickPitchRotationRemainig ? RecoilKickPitchRotationRemainig : RecoilKickPitchRotationStep;

		RecoilKickMovement += RecoilKickMovmentToAdd;
		RecoilKickPitchRotation += RecoilPitchRotationToAdd;

		if (RecoilKickMovement == RecoilKickMaxDistance && RecoilKickPitchRotation == RecoilKickMaxPitchRotation)
		{
			bIsRecoilKick = false;
			bIsRecoilRecovery = true;
		}
	}
	else if (bIsRecoilRecovery)
	{
		float RecoilKickMaxDistance = Weapon != nullptr ? Weapon->GetRecoilKickMaxDistance() : 0.0f;
		float RecoilKickMovementStep = (RecoilKickMaxDistance / RecoilRecoveryTotalTime) * DeltaSeconds;

		float RecoilKickMaxPitchRotation = Weapon != nullptr ? Weapon->GetRecoilKickMaxPitchRotation() : 0.0f;
		float RecoilKickPitchRotationStep = (RecoilKickMaxPitchRotation / RecoilRecoveryTotalTime) * DeltaSeconds;

		float RecoilKickMovementToSubtract = RecoilKickMovementStep > RecoilKickMovement ? RecoilKickMovement : RecoilKickMovementStep;;
		float RecoilPitchRotationToSubtract = RecoilKickPitchRotationStep > RecoilKickPitchRotation ? RecoilKickPitchRotation : RecoilKickPitchRotationStep;

		RecoilKickMovement -= RecoilKickMovementToSubtract;
		RecoilKickPitchRotation -= RecoilPitchRotationToSubtract;

		if (RecoilKickMovement == 0.0f && RecoilKickPitchRotation == 0.0f)
		{
			bIsRecoilRecovery = false;
		}
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

	WeaponRootAnimTransform = FTransform(FRotator(ProceduralAnimRollRotation + SwayRollRotation, 0.0f, -RecoilKickPitchRotation), FVector(ProceduralAnimHorizontalMovement + SwayHorizontalMovement, -RecoilKickMovement, ProceduralAnimVerticalMovement + SwayVerticalMovement), FVector::OneVector);
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
