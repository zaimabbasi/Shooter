// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/HandsAnimInstance.h"
#include "Character/ShooterCharacter.h"
#include "Types/ShooterNames.h"
#include "Utility/ShooterUtility.h"

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
	LeaningAngle = ShooterCharacter->GetLeaningAngle();
	CombatAction = ShooterCharacter->GetCombatAction();
	bIsTransition = ShooterCharacter->GetIsTransition();
	bHasVelocity = ShooterCharacter->GetVelocity().SizeSquared2D() > 0.0f;

	//bIsThirdAction = bIsTransition || bIsSprinting || (bIsProned && bHasVelocity);
	bIsThirdAction = bIsSprinting || (bIsProned && bHasVelocity);

	CalculateTransforms();
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

void UHandsAnimInstance::CalculateRootBoneLocation()
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

void UHandsAnimInstance::CalculateWeaponRootAnimTransform()
{
	if (bIsThirdAction && CharacterMesh && HandsMesh)
	{
		WeaponRootAnimTransform = CharacterMesh->GetSocketTransform(WEAPON_ROOT_3RD_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		WeaponRootAnimTransform = FShooterUtility::TransformToBoneSpace(HandsMesh, WEAPON_ROOT_SOCKET_NAME, WeaponRootAnimTransform);
	}
}

void UHandsAnimInstance::CalculateBendGoalLeftTransform()
{
	if (bIsThirdAction && CharacterMesh && HandsMesh)
	{
		BendGoalLeftTransform = CharacterMesh->GetSocketTransform(BEND_GOAL_LEFT_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		BendGoalLeftTransform = FShooterUtility::TransformToBoneSpace(HandsMesh, WEAPON_SOCKET_NAME, BendGoalLeftTransform);
	}
}

void UHandsAnimInstance::CalculateBendGoalRightTransform()
{
	if (bIsThirdAction && CharacterMesh && HandsMesh)
	{
		BendGoalRightTransform = CharacterMesh->GetSocketTransform(BEND_GOAL_RIGHT_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		BendGoalRightTransform = FShooterUtility::TransformToBoneSpace(HandsMesh, WEAPON_SOCKET_NAME, BendGoalRightTransform);
	}
}

void UHandsAnimInstance::CalculateWeaponLCollarboneMarkerTransform()
{
	if (bIsThirdAction && CharacterMesh && HandsMesh)
	{
		WeaponLCollarboneMarkerTransform = CharacterMesh->GetSocketTransform(L_COLLARBONE_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		WeaponLCollarboneMarkerTransform = FShooterUtility::TransformToBoneSpace(HandsMesh, WEAPON_SOCKET_NAME, WeaponLCollarboneMarkerTransform);
	}
}

void UHandsAnimInstance::CalculateWeaponRCollarboneMarkerTransform()
{
	if (bIsThirdAction && CharacterMesh && HandsMesh)
	{
		WeaponRCollarboneMarkerTransform = CharacterMesh->GetSocketTransform(R_COLLARBONE_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		WeaponRCollarboneMarkerTransform = FShooterUtility::TransformToBoneSpace(HandsMesh, WEAPON_SOCKET_NAME, WeaponRCollarboneMarkerTransform);
	}
}

void UHandsAnimInstance::CalculateWeaponLHandMarkerTransform()
{
	if (bIsThirdAction && CharacterMesh && HandsMesh)
	{
		WeaponLHandMarkerTransform = CharacterMesh->GetSocketTransform(IK_S_L_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		WeaponLHandMarkerTransform = FShooterUtility::TransformToBoneSpace(HandsMesh, WEAPON_SOCKET_NAME, WeaponLHandMarkerTransform);
	}
}

void UHandsAnimInstance::CalculateWeaponRHandMarkerTransform()
{
	if (bIsThirdAction && CharacterMesh && HandsMesh)
	{
		WeaponRHandMarkerTransform = CharacterMesh->GetSocketTransform(IK_S_R_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		WeaponRHandMarkerTransform = FShooterUtility::TransformToBoneSpace(HandsMesh, WEAPON_SOCKET_NAME, WeaponRHandMarkerTransform);
	}
}

void UHandsAnimInstance::CalculateTransforms()
{
	CalculateRootBoneLocation();

	CalculateWeaponRootAnimTransform();

	CalculateBendGoalLeftTransform();
	CalculateBendGoalRightTransform();

	CalculateWeaponLCollarboneMarkerTransform();
	CalculateWeaponRCollarboneMarkerTransform();

	CalculateWeaponLHandMarkerTransform();
	CalculateWeaponRHandMarkerTransform();
}
