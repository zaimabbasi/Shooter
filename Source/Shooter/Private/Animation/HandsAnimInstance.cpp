// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/HandsAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/ShooterCharacter.h"
//#include "Components/CharacterCombatComponent.h"
#include "Types/ShooterNames.h"
#include "Utility/ShooterUtility.h"
#include "Weapon/Weapon.h"

void UHandsAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	OwningComponent = GetOwningComponent();

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

	//Controller = ShooterCharacter->GetController();
	//EquippedWeapon = ShooterCharacter->GetEquippedWeapon();
	//bIsLocallyControlled = ShooterCharacter->IsLocallyControlled();
	//bIsTransition = ShooterCharacter->GetIsTransition();
	//RemoteViewYaw = ShooterCharacter->RemoteViewYaw;
	//BaseAimRotation = ShooterCharacter->GetBaseAimRotation();
	//ControlRotation = ShooterCharacter->GetControlRotation();
	//ActorRotation = ShooterCharacter->GetActorRotation();
	//Velocity = ShooterCharacter->GetVelocity();

	AWeapon* EquippedWeapon = ShooterCharacter->GetEquippedWeapon();
	bool bIsLocallyControlled = ShooterCharacter->IsLocallyControlled();
	bool bIsTransition = ShooterCharacter->GetIsTransition();
	bool bHasController = ShooterCharacter->GetController() != nullptr;
	float ActorRotationYaw = ShooterCharacter->GetActorRotation().Yaw;
	//float RemoteViewYaw = ShooterCharacter->RemoteViewYaw;
	float LeaningTargetAngle = ShooterCharacter->GetLeaningTargetAngle();
	float LeaningInterpSpeed = ShooterCharacter->GetLeaningInterpSpeed();
	FVector Velocity = ShooterCharacter->GetVelocity();
	FRotator BaseAimRotation = ShooterCharacter->GetBaseAimRotation();
	FRotator ControlRotation = ShooterCharacter->GetControlRotation();

	CharacterMesh = ShooterCharacter->GetMesh();
	WeaponMesh = EquippedWeapon != nullptr ? EquippedWeapon->GetMesh() : nullptr;
	bHasVelocity = Velocity.SizeSquared2D() > 0.0f;
	bIsProned = ShooterCharacter->bIsProned;
	bIsSprinting = ShooterCharacter->bIsSprinting;
	bIsAiming = ShooterCharacter->GetIsAiming();
	bIsThirdAction = bIsTransition || bIsSprinting || (bIsProned && bHasVelocity);
	bIsWeaponEquipped = EquippedWeapon != nullptr;
	
	/*AO_Pitch = bIsLocallyControlled ? ControlRotation.Pitch : BaseAimRotation.Pitch;
	if (AO_Pitch > 90.0f)
	{
		FVector2D InRange = FVector2D(270.0, 360.0);
		FVector2D OutRange = FVector2D(-90.0, 0.0);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}*/
	AO_Pitch = ShooterCharacter->GetAO_Pitch();

	/*float ViewYaw = bIsLocallyControlled ? ControlRotation.Yaw : bHasController ? BaseAimRotation.Yaw : FRotator::DecompressAxisFromByte(RemoteViewYaw);
	AO_Yaw = UKismetMathLibrary::NormalizedDeltaRotator(FRotator(0.0f, ViewYaw, 0.0f), FRotator(0.0f, ActorRotationYaw, 0.0f)).Yaw;*/
	AO_Yaw = ShooterCharacter->GetAO_Yaw();

	LeaningAngle = FMath::FInterpTo(LeaningAngle, LeaningTargetAngle, DeltaSeconds, LeaningInterpSpeed);

	CombatAction = ShooterCharacter->GetCombatAction();

}

void UHandsAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (ShooterCharacter == nullptr)
	{
		return;
	}

	IKAlpha = bIsThirdAction && !bIsWeaponEquipped ? 1.0f : 0.0f;

	if (CharacterMesh)
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
