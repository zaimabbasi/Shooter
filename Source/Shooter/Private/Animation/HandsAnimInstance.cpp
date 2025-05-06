// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/HandsAnimInstance.h"
#include "Character/ShooterCharacter.h"
#include "Types/ShooterNames.h"
#include "Utility/ShooterUtility.h"
#include "Weapon/Weapon.h"

void UHandsAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	HandsMesh = GetOwningComponent();

}

void UHandsAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
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
	bIsAiming = ShooterCharacter->GetIsAiming();
	bIsProned = ShooterCharacter->bIsProned;
	bIsSprinting = ShooterCharacter->bIsSprinting;
	AO_Pitch = ShooterCharacter->GetAO_Pitch();
	AO_Yaw = ShooterCharacter->GetAO_Yaw();
	CombatAction = ShooterCharacter->GetCombatAction();

	AWeapon* EquippedWeapon = ShooterCharacter->GetEquippedWeapon();
	WeaponMesh = EquippedWeapon != nullptr ? EquippedWeapon->GetMesh() : nullptr;
	bIsWeaponEquipped = EquippedWeapon != nullptr;

	bHasVelocity = ShooterCharacter->GetVelocity().SizeSquared2D() > 0.0f;

	bool bIsTransition = ShooterCharacter->GetIsTransition();
	bIsThirdAction = bIsTransition || bIsSprinting || (bIsProned && bHasVelocity);

	IKAlpha = bIsThirdAction && !bIsWeaponEquipped ? 1.0f : 0.0f;

	float LeaningTargetAngle = ShooterCharacter->GetLeaningTargetAngle();
	float LeaningInterpSpeed = ShooterCharacter->GetLeaningInterpSpeed();
	LeaningAngle = FMath::FInterpTo(LeaningAngle, LeaningTargetAngle, DeltaSeconds, LeaningInterpSpeed);

	// Old
	/*if (CharacterMesh)
	{
		RibcageTransform = CharacterMesh->GetSocketTransform(BASE_HUMAN_RIBCAGE_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		RibcageTransform = FShooterUtility::TransformToBoneSpace(CharacterMesh, ROOT_JOINT_SOCKET_NAME, RibcageTransform);

		if (bIsThirdAction && !bIsWeaponEquipped)
		{
			LCollarboneTransform = CharacterMesh->GetSocketTransform(L_COLLARBONE_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
			RCollarboneTransform = CharacterMesh->GetSocketTransform(R_COLLARBONE_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);

			WeaponRootAnimTransform = CharacterMesh->GetSocketTransform(WEAPON_ROOT_3RD_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);

			BendGoalLeftTransform = CharacterMesh->GetSocketTransform(BEND_GOAL_LEFT_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
			BendGoalRightTransform = CharacterMesh->GetSocketTransform(BEND_GOAL_RIGHT_SOCKET_NAME, ERelativeTransformSpace::RTS_World);

			LPalmTransform = CharacterMesh->GetSocketTransform(IK_S_L_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
			RPalmTransform = CharacterMesh->GetSocketTransform(IK_S_R_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);

		}
	}*/
	// New
	if (CharacterMesh)
	{
		RibcageTransform = CharacterMesh->GetSocketTransform(BASE_HUMAN_RIBCAGE_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		RibcageTransform = FShooterUtility::TransformToBoneSpace(CharacterMesh, ROOT_JOINT_SOCKET_NAME, RibcageTransform);

		if (HandsMesh && bIsThirdAction && !bIsWeaponEquipped)
		{
			LCollarboneTransform = CharacterMesh->GetSocketTransform(L_COLLARBONE_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
			LCollarboneTransform = FShooterUtility::TransformToBoneSpace(HandsMesh, BASE_HUMAN_RIBCAGE_SOCKET_NAME, LCollarboneTransform);

			RCollarboneTransform = CharacterMesh->GetSocketTransform(R_COLLARBONE_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
			RCollarboneTransform = FShooterUtility::TransformToBoneSpace(HandsMesh, BASE_HUMAN_RIBCAGE_SOCKET_NAME, RCollarboneTransform);

			WeaponRootAnimTransform = CharacterMesh->GetSocketTransform(WEAPON_ROOT_3RD_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
			WeaponRootAnimTransform = FShooterUtility::TransformToBoneSpace(HandsMesh, BASE_HUMAN_RIBCAGE_SOCKET_NAME, WeaponRootAnimTransform);

			BendGoalLeftTransform = CharacterMesh->GetSocketTransform(BEND_GOAL_LEFT_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
			BendGoalLeftTransform = FShooterUtility::TransformToBoneSpace(HandsMesh, BASE_HUMAN_RIBCAGE_SOCKET_NAME, BendGoalLeftTransform);

			BendGoalRightTransform = CharacterMesh->GetSocketTransform(BEND_GOAL_RIGHT_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
			BendGoalRightTransform = FShooterUtility::TransformToBoneSpace(HandsMesh, BASE_HUMAN_RIBCAGE_SOCKET_NAME, BendGoalRightTransform);

			LPalmTransform = CharacterMesh->GetSocketTransform(IK_S_L_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
			LPalmTransform = FShooterUtility::TransformToBoneSpace(HandsMesh, BASE_HUMAN_RIBCAGE_SOCKET_NAME, LPalmTransform);

			RPalmTransform = CharacterMesh->GetSocketTransform(IK_S_R_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
			RPalmTransform = FShooterUtility::TransformToBoneSpace(HandsMesh, BASE_HUMAN_RIBCAGE_SOCKET_NAME, RPalmTransform);

		}
	}

}

void UHandsAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (ShooterCharacter == nullptr)
	{
		return;
	}

}

void UHandsAnimInstance::AnimNotify_Idle() const
{
	OnHandsAnimInstanceIdle.Broadcast();
}

void UHandsAnimInstance::AnimNotify_IdleToOut() const
{
	OnHandsAnimInstanceIdleToOut.Broadcast();
}

void UHandsAnimInstance::AnimNotify_Out() const
{
	OnHandsAnimInstanceOut.Broadcast();
}

void UHandsAnimInstance::AnimNotify_OutToIdle() const
{
	OnHandsAnimInstanceOutToIdle.Broadcast();
}
