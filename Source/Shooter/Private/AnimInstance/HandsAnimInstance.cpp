// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/HandsAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Character/ShooterCharacter.h"
#include "Enum/LeanDirection.h"
#include "Struct/ShooterUtility.h"
#include "Type/ShooterNameType.h"

void UHandsAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
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

	USkeletalMeshComponent* HandsMesh = ShooterCharacter->GetHandsMesh();
	UCameraComponent* FirstPersonCamera = ShooterCharacter->GetFirstPersonCamera();
	FVector FPCameraDesiredLocationOffset = ShooterCharacter->GetFPCameraDesiredLocationOffset();
	float FPCameraDesiredFOV = ShooterCharacter->CalculateFPCameraDesiredFOV();
	CharacterMesh = ShooterCharacter->GetMesh();
	AO_Yaw = ShooterCharacter->GetAO_Yaw(AO_Yaw, DeltaSeconds);
	AO_Pitch = ShooterCharacter->GetAO_Pitch(AO_Pitch, DeltaSeconds);
	bIsAiming = ShooterCharacter->GetIsAiming();
	WeaponMesh = ShooterCharacter->GetEquippedWeaponMesh();
	bIsWeaponEquipped = ShooterCharacter->IsWeaponEquipped();
	CombatAction = ShooterCharacter->GetCombatAction();
	bHasVelocity = ShooterCharacter->GetVelocity().SizeSquared2D() > 0.0f;
	bIsProned = ShooterCharacter->bIsProned;
	bIsSprinting = ShooterCharacter->bIsSprinting;
	bIsThirdAction = ShooterCharacter->IsThirdAction();
	IKAlpha = bIsThirdAction ? 1.0f : 0.0f;

	if (CharacterMesh)
	{
		RibcageTransform = CharacterMesh->GetSocketTransform(BASE_HUMAN_RIBCAGE_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		RibcageTransform = FShooterUtility::TransformToBoneSpace(CharacterMesh, CharacterMesh->GetBoneName(0), RibcageTransform);
	}

	ELeanDirection LeanDirection = ShooterCharacter->GetLeanDirection();
	float TargetLean = 0.0f;
	if (LeanDirection == ELeanDirection::LD_Left)
	{
		TargetLean = -ShooterCharacter->GetMaxLean();
	}
	else if (LeanDirection == ELeanDirection::LD_Right)
	{
		TargetLean = ShooterCharacter->GetMaxLean();
	}
	float DeltaLean = TargetLean - Lean;
	float LeanStep = ShooterCharacter->GetLeaningRate() * DeltaSeconds;
	if (FMath::Abs(DeltaLean) < LeanStep)
	{
		Lean = TargetLean;
	}
	else
	{
		if (DeltaLean < 0.0f)
		{
			LeanStep = -LeanStep;
		}
		Lean += LeanStep;
	}

	float InterpSpeed = 5.0f;
	if (FirstPersonCamera)
	{
		FirstPersonCamera->SetRelativeLocation(FMath::VInterpTo(FirstPersonCamera->GetRelativeLocation(), FPCameraDesiredLocationOffset, DeltaSeconds, InterpSpeed));
		FirstPersonCamera->SetFieldOfView(FMath::FInterpTo(FirstPersonCamera->FieldOfView, FPCameraDesiredFOV, DeltaSeconds, InterpSpeed));
	}

	if (CharacterMesh && HandsMesh && bIsThirdAction && !bIsWeaponEquipped)
	{
		BendGoalLeftTransform = CharacterMesh->GetSocketTransform(BEND_GOAL_LEFT_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		BendGoalRightTransform = CharacterMesh->GetSocketTransform(BEND_GOAL_RIGHT_SOCKET_NAME, ERelativeTransformSpace::RTS_World);

		LCollarboneTransform = CharacterMesh->GetSocketTransform(L_COLLARBONE_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		RCollarboneTransform = CharacterMesh->GetSocketTransform(R_COLLARBONE_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);

		LPalmTransform = CharacterMesh->GetSocketTransform(IK_S_L_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		RPalmTransform = CharacterMesh->GetSocketTransform(IK_S_R_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);

		WeaponRootAnimTransform = CharacterMesh->GetSocketTransform(WEAPON_ROOT_3RD_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
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
