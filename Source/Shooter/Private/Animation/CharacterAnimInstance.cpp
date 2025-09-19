// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/CharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/ShooterCharacter.h"
#include "Components/CharacterCombatComponent.h"
#include "Types/CharacterTypes.h"
#include "Types/ShooterNames.h"
#include "Utility/ShooterUtility.h"
#include "Weapon/Weapon.h"

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	CharacterMesh = GetOwningComponent();

	bIsThirdAction = true;

}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
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

	HandsMesh = ShooterCharacter->GetHandsMesh();
	bIsCrouched = ShooterCharacter->bIsCrouched;
	bIsProned = ShooterCharacter->bIsProned;
	bIsSlowing = ShooterCharacter->bIsSlowing;
	bIsSprinting = ShooterCharacter->bIsSprinting;
	AO_Pitch = ShooterCharacter->GetAO_Pitch();
	AO_Yaw = ShooterCharacter->GetAO_Yaw();
	RootBoneYaw = ShooterCharacter->GetRootBoneYaw();
	VelocityYawOffset = ShooterCharacter->GetVelocityYawOffset();
	LeaningTransitionDuration = ShooterCharacter->GetLeaningTransitionDuration();
	CombatAction = ShooterCharacter->GetCombatAction();
	LeaningDirection = ShooterCharacter->GetLeaningDirection();
	TurningDirection = ShooterCharacter->GetTurningDirection();
	bIsTransition = ShooterCharacter->GetIsTransition();

	bIsAccelerating = ShooterCharacter->GetCharacterMovement() && ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().SizeSquared2D() > 0.0f;
	bHasVelocity = ShooterCharacter->GetVelocity().SizeSquared2D() > 0.0f;

	//bIsThirdAction = bIsTransition || bIsSprinting || (bIsProned && bHasVelocity);
	bIsThirdAction = bIsSprinting || (bIsProned && bHasVelocity);

	AWeapon* EquippedWeapon = ShooterCharacter->GetEquippedWeapon();
	bIsWeaponEquipped = EquippedWeapon != nullptr;
	bIsEquippedWeaponPistol = EquippedWeapon && EquippedWeapon->IsPistol();
	bIsEquippedWeaponOneHanded = EquippedWeapon && EquippedWeapon->GetIsOneHanded();

	CalculateTransforms();
}

void UCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (ShooterCharacter == nullptr)
	{
		return;
	}

}

void UCharacterAnimInstance::AnimNotify_IdleAimToTransitionIdleAimToSprintSlowStarted() const
{
	OnCharacterAnimInstanceIdleAimToTransitionIdleAimToSprintSlowStarted.Broadcast();
}

void UCharacterAnimInstance::AnimNotify_IdleAimToTransitionIdleAimToProneIdleAimStarted() const
{
	OnCharacterAnimInstanceIdleAimToTransitionIdleAimToProneIdleAimStarted.Broadcast();
}

void UCharacterAnimInstance::AnimNotify_SprintSlowToTransitionSprintSlowToCrouchIdleAimStarted() const
{
	OnCharacterAnimInstanceSprintSlowToTransitionSprintSlowToCrouchIdleAimStarted.Broadcast();
}

void UCharacterAnimInstance::AnimNotify_SprintSlowToTransitionSprintSlowToIdleAimStarted() const
{
	OnCharacterAnimInstanceSprintSlowToTransitionSprintSlowToIdleAimStarted.Broadcast();
}

void UCharacterAnimInstance::AnimNotify_SprintSlowToTransitionSprintSlowToProneIdleAimStarted() const
{
	OnCharacterAnimInstanceSprintSlowToTransitionSprintSlowToProneIdleAimStarted.Broadcast();
}

void UCharacterAnimInstance::AnimNotify_WalkAimToTransitionIdleAimToProneIdleAimStarted() const
{
	OnCharacterAnimInstanceWalkAimToTransitionIdleAimToProneIdleAimStarted.Broadcast();
}

void UCharacterAnimInstance::AnimNotify_WalkAimSlowToTransitionIdleAimToProneIdleAimStarted() const
{
	OnCharacterAnimInstanceWalkAimSlowToTransitionIdleAimToProneIdleAimStarted.Broadcast();
}

void UCharacterAnimInstance::AnimNotify_CrouchAimToTransitionIdleLowAimToProneIdleAimStarted() const
{
	OnCharacterAnimInstanceCrouchAimToTransitionIdleLowAimToProneIdleAimStarted.Broadcast();
}

void UCharacterAnimInstance::AnimNotify_CrouchAimSlowToTransitionIdleLowAimToProneIdleAimStarted() const
{
	OnCharacterAnimInstanceCrouchAimSlowToTransitionIdleLowAimToProneIdleAimStarted.Broadcast();
}

void UCharacterAnimInstance::AnimNotify_IdleLowAimToTransitionIdleLowAimToProneIdleAimStarted() const
{
	OnCharacterAnimInstanceIdleLowAimToTransitionIdleLowAimToProneIdleAimStarted.Broadcast();
}

void UCharacterAnimInstance::AnimNotify_ProneFastToTransitionProneIdleAimToIdleAimStarted() const
{
	OnCharacterAnimInstanceProneFastToTransitionProneIdleAimToIdleAimStarted.Broadcast();
}

void UCharacterAnimInstance::AnimNotify_ProneFastToTransitionProneIdleAimToIdleLowAimStarted() const
{
	OnCharacterAnimInstanceProneFastToTransitionProneIdleAimToIdleLowAimStarted.Broadcast();
}

void UCharacterAnimInstance::AnimNotify_ProneIdleAimToTransitionProneIdleAimToIdleAimStarted() const
{
	OnCharacterAnimInstanceProneIdleAimToTransitionProneIdleAimToIdleAimStarted.Broadcast();
}

void UCharacterAnimInstance::AnimNotify_ProneIdleAimToTransitionProneIdleAimToIdleLowAimStarted() const
{
	OnCharacterAnimInstanceProneIdleAimToTransitionProneIdleAimToIdleLowAimStarted.Broadcast();
}

void UCharacterAnimInstance::AnimNotify_TransitionIdleAimToProneIdleAimToProneIdleAimStarted() const
{
	OnCharacterAnimInstanceTransitionIdleAimToProneIdleAimToProneIdleAimStarted.Broadcast();
}

void UCharacterAnimInstance::AnimNotify_TransitionIdleAimToSprintSlowToSprintSlowStarted() const
{
	OnCharacterAnimInstanceTransitionIdleAimToSprintSlowToSprintSlowStarted.Broadcast();
}

void UCharacterAnimInstance::AnimNotify_TransitionIdleLowAimToProneIdleAimToProneIdleAimStarted() const
{
	OnCharacterAnimInstanceTransitionIdleLowAimToProneIdleAimToProneIdleAimStarted.Broadcast();
}

void UCharacterAnimInstance::AnimNotify_TransitionProneIdleAimToIdleAimToIdleAimStarted() const
{
	OnCharacterAnimInstanceTransitionProneIdleAimToIdleAimToIdleAimStarted.Broadcast();
}

void UCharacterAnimInstance::AnimNotify_TransitionProneIdleAimToIdleLowAimToIdleLowAimStarted() const
{
	OnCharacterAnimInstanceTransitionProneIdleAimToIdleLowAimToIdleLowAimStarted.Broadcast();
}

void UCharacterAnimInstance::AnimNotify_TransitionSprintSlowToCrouchIdleAimToIdleLowAimStarted() const
{
	OnCharacterAnimInstanceTransitionSprintSlowToCrouchIdleAimToIdleLowAimStarted.Broadcast();
}

void UCharacterAnimInstance::AnimNotify_TransitionSprintSlowToIdleAimToIdleAimStarted() const
{
	OnCharacterAnimInstanceTransitionSprintSlowToIdleAimToIdleAimStarted.Broadcast();
}

void UCharacterAnimInstance::AnimNotify_TransitionSprintSlowToIdleAimToIdleLowAimStarted() const
{
	OnCharacterAnimInstanceTransitionSprintSlowToIdleAimToIdleLowAimStarted.Broadcast();
}

void UCharacterAnimInstance::AnimNotify_TransitionSprintSlowToProneIdleAimToProneIdleAimStarted() const
{
	OnCharacterAnimInstanceTransitionSprintSlowToProneIdleAimToProneIdleAimStarted.Broadcast();
}

void UCharacterAnimInstance::CalculateBendGoalLeftTransform()
{
	if (!bIsThirdAction && CharacterMesh && HandsMesh)
	{
		BendGoalLeftTransform = HandsMesh->GetSocketTransform(BEND_GOAL_LEFT_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		BendGoalLeftTransform = FShooterUtility::TransformToBoneSpace(CharacterMesh, BASE_HUMAN_SPINE3_SOCKET_NAME, BendGoalLeftTransform);
	}
}

void UCharacterAnimInstance::CalculateBendGoalRightTransform()
{
	if (!bIsThirdAction && CharacterMesh && HandsMesh)
	{
		BendGoalRightTransform = HandsMesh->GetSocketTransform(BEND_GOAL_RIGHT_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		BendGoalRightTransform = FShooterUtility::TransformToBoneSpace(CharacterMesh, BASE_HUMAN_SPINE3_SOCKET_NAME, BendGoalRightTransform);
	}
}

void UCharacterAnimInstance::CalculateIKSLPalmTransform()
{
	if (!bIsThirdAction && CharacterMesh && HandsMesh)
	{
		IKSLPalmTransform = HandsMesh->GetSocketTransform(BASE_HUMAN_L_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		IKSLPalmTransform = FShooterUtility::TransformToBoneSpace(CharacterMesh, BASE_HUMAN_SPINE3_SOCKET_NAME, IKSLPalmTransform);
	}
}

void UCharacterAnimInstance::CalculateIKSRPalmTransform()
{
	if (!bIsThirdAction && CharacterMesh && HandsMesh)
	{
		IKSRPalmTransform = HandsMesh->GetSocketTransform(BASE_HUMAN_R_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		IKSRPalmTransform = FShooterUtility::TransformToBoneSpace(CharacterMesh, BASE_HUMAN_SPINE3_SOCKET_NAME, IKSRPalmTransform);
	}
}

void UCharacterAnimInstance::CalculateTransforms()
{
	CalculateBendGoalLeftTransform();
	CalculateBendGoalRightTransform();

	CalculateIKSLPalmTransform();
	CalculateIKSRPalmTransform();
}
