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
	OwningComponent = GetOwningComponent();

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

	//Controller = ShooterCharacter->GetController();
	//EquippedWeapon = ShooterCharacter->GetEquippedWeapon();
	//bIsLocallyControlled = ShooterCharacter->IsLocallyControlled();
	//bIsTransition = ShooterCharacter->GetIsTransition();
	//RemoteViewYaw = ShooterCharacter->RemoteViewYaw;
	//BaseAimRotation = ShooterCharacter->GetBaseAimRotation();
	//ControlRotation = ShooterCharacter->GetControlRotation();
	//ActorRotation = ShooterCharacter->GetActorRotation();
	//Velocity = ShooterCharacter->GetVelocity();
	//CurrentAcceleration = ShooterCharacter->GetCurrentAcceleration();

	/*if ((bHasVelocity && !ShooterCharacter->bUseControllerRotationYaw) || TurningDirection != ETurningDirection::TD_None)
	{
		ActorRotationInterp = FMath::RInterpTo(ActorRotationInterp, ActorRotationTarget, DeltaSeconds, ActorRotationInterpSpeed);
	}*/

	AWeapon* EquippedWeapon = ShooterCharacter->GetEquippedWeapon();
	bool bIsLocallyControlled = ShooterCharacter->IsLocallyControlled();
	bool bIsTransition = ShooterCharacter->GetIsTransition();
	bool bHasController = ShooterCharacter->GetController() != nullptr;
	//float RemoteViewYaw = ShooterCharacter->RemoteViewYaw;
	FVector Velocity = ShooterCharacter->GetVelocity();
	FRotator ActorRotation = ShooterCharacter->GetActorRotation();
	FRotator BaseAimRotation = ShooterCharacter->GetBaseAimRotation();
	FRotator ControlRotation = ShooterCharacter->GetControlRotation();

	HandsMesh = ShooterCharacter->GetHandsMesh();
	bIsAccelerating = ShooterCharacter->GetCharacterMovement() && ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().SizeSquared2D() > 0.0f;
	bHasVelocity = Velocity.SizeSquared2D() > 0.0f;
	bIsCrouched = ShooterCharacter->bIsCrouched;
	bIsProned = ShooterCharacter->bIsProned;
	bIsSlowing = ShooterCharacter->bIsSlowing;
	bIsSprinting = ShooterCharacter->bIsSprinting;
	bIsThirdAction = bIsTransition || bIsSprinting || (bIsProned && bHasVelocity);
	bIsWeaponEquipped = EquippedWeapon != nullptr;
	bIsEquippedWeaponPistol = EquippedWeapon && EquippedWeapon->IsPistol();
	bIsEquippedWeaponOneHanded = EquippedWeapon && EquippedWeapon->GetIsOneHanded();
	LeaningTransitionDuration = ShooterCharacter->GetLeaningTransitionDuration();

	/*AO_Pitch = bIsLocallyControlled ? ControlRotation.Pitch : BaseAimRotation.Pitch;
	if (AO_Pitch > 90.0f)
	{
		FVector2D InRange = FVector2D(270.0, 360.0);
		FVector2D OutRange = FVector2D(-90.0, 0.0);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}*/
	AO_Pitch = ShooterCharacter->GetAO_Pitch();

	/*float ViewYaw = bIsLocallyControlled ? ControlRotation.Yaw : bHasController ? BaseAimRotation.Yaw : FRotator::DecompressAxisFromByte(RemoteViewYaw);
	AO_Yaw = UKismetMathLibrary::NormalizedDeltaRotator(FRotator(0.0f, ViewYaw, 0.0f), FRotator(0.0f, ActorRotation.Yaw, 0.0f)).Yaw;*/
	AO_Yaw = ShooterCharacter->GetAO_Yaw();

	RootJointYaw = ShooterCharacter->GetRootJointYaw();
	
	if (bHasVelocity)
	{
		float VelocityYaw = UKismetMathLibrary::MakeRotFromX(Velocity).Yaw;
		VelocityYawOffset = UKismetMathLibrary::NormalizedDeltaRotator(FRotator(0.0, VelocityYaw, 0.0), FRotator(0.0, ActorRotation.Yaw, 0.0)).Yaw;
	}

	CombatAction = ShooterCharacter->GetCombatAction();
	LeaningDirection = ShooterCharacter->GetLeaningDirection();

	TurningDirection = ShooterCharacter->GetTurningDirection();

	//if (bHasVelocity)
	//{
	//	if (TurningDirection != ETurningDirection::TD_None)
	//	{
	//		TurningDirection = ETurningDirection::TD_None;
	//	}

	//	if (FMath::Abs(AO_Yaw) < ShooterCharacter->AllowedYawError)
	//	{
	//		if (!ShooterCharacter->bUseControllerRotationYaw)
	//		{
	//			ShooterCharacter->bUseControllerRotationYaw = true;
	//		}
	//	}
	//}
	//else
	//{
	//	float AllowedAO_Yaw = (!bIsCrouched && !bIsProned) || bIsCrouched ? 90.0f : 45.0f;
	//	if (FMath::Abs(AO_Yaw) > AllowedAO_Yaw && TurningDirection == ETurningDirection::TD_None)
	//	{
	//		TurningDirection = AO_Yaw > 0.0f ? ETurningDirection::TD_Right : ETurningDirection::TD_Left;
	//	}
	//	/*else if (FMath::Abs(AO_Yaw) < ShooterCharacter->AllowedYawError && TurningDirection != ETurningDirection::TD_None)
	//	{
	//		TurningDirection = ETurningDirection::TD_None;
	//	}*/
	//	
	//	if (ShooterCharacter->bUseControllerRotationYaw)
	//	{
	//		ShooterCharacter->bUseControllerRotationYaw = false;
	//	}

	//}

	//float ExceedingAO_Yaw = 0.0f;
	//float ActorRotationYawInterpSpeed = 0.0f;
	//if (bHasVelocity)
	//{
	//	ExceedingAO_Yaw = AO_Yaw;
	//	ActorRotationYawInterpSpeed = 15.0f;
	//}
	//else if (TurningDirection != ETurningDirection::TD_None)
	//{
	//	//ExceedingAO_Yaw = AO_Yaw < -AllowedAO_Yaw ? AO_Yaw + AllowedAO_Yaw : AO_Yaw > AllowedAO_Yaw ? AO_Yaw - AllowedAO_Yaw : 0.0f;
	//	ExceedingAO_Yaw = AO_Yaw;
	//	ActorRotationYawInterpSpeed = 5.0f;
	//}

	//if (!FMath::IsNearlyZero(ExceedingAO_Yaw))
	//{
	//	ShooterCharacter->AddActorLocalRotation(FRotator(0.0f, ExceedingAO_Yaw, 0.0f));
	//	AO_Yaw -= ExceedingAO_Yaw;
	//}


	/*if (TurningDirection != ETurningDirection::TD_None)
	{
		float ExceedingYaw = AO_Yaw < -AllowedAO_Yaw ? AO_Yaw + AllowedAO_Yaw : AO_Yaw > AllowedAO_Yaw ? AO_Yaw - AllowedAO_Yaw : 0.0f;
		if (!FMath::IsNearlyZero(ExceedingYaw))
		{
			ShooterCharacter->SetActorRotation(FRotator(0.0f, ShooterCharacter->GetActorRotation().Yaw + ExceedingYaw, 0.0f));
		}
	}*/

	/*UCharacterMovementComponent* MovementComponent = ShooterCharacter->GetCharacterMovement();
	if (bHasVelocity)
	{
		if (TurningDirection != ETurningDirection::TD_None)
		{
			OnCharacterAnimInstanceTurningInPlace.Broadcast(ETurningDirection::TD_None);
		}

		if (FMath::Abs(AO_Yaw) < ShooterCharacter->AllowedYawError)
		{
			ShooterCharacter->bUseControllerRotationYaw = true;
			if (MovementComponent)
			{
				MovementComponent->bUseControllerDesiredRotation = false;
			}
		}
		else if (MovementComponent)
		{
			if (!MovementComponent->bUseControllerDesiredRotation || MovementComponent->RotationRate.Yaw != CharacterMovementRotationRateYaw)
			{
				MovementComponent->bUseControllerDesiredRotation = true;
				MovementComponent->RotationRate.Yaw = CharacterMovementRotationRateYaw;
			}
		}
	}
	else
	{
		if (ShooterCharacter->bUseControllerRotationYaw)
		{
			ShooterCharacter->bUseControllerRotationYaw = false;
		}

		if (FMath::Abs(AO_Yaw) > AllowedAO_Yaw)
		{
			if (MovementComponent)
			{
				MovementComponent->bUseControllerDesiredRotation = true;
				MovementComponent->RotationRate.Yaw = TurnInPlaceRotationRateYaw;
			}

			OnCharacterAnimInstanceTurningInPlace.Broadcast(AO_Yaw > 0 ? ETurningDirection::TD_Right : ETurningDirection::TD_Left);
		}
		else if (FMath::Abs(AO_Yaw) < ShooterCharacter->AllowedYawError)
		{
			if (MovementComponent)
			{
				MovementComponent->bUseControllerDesiredRotation = false;
			}

			OnCharacterAnimInstanceTurningInPlace.Broadcast(ETurningDirection::TD_None);
		}
	}*/

	/*if (CombatAction == ECombatAction::CA_Idle && !bIsThirdAction)
	{
		if (!bHasVelocity && !ShooterCharacter->IsPlayingIdleAnimTimeline())
		{
			OnCharacterAnimInstanceIdle.Broadcast();
		}
		else if (bHasVelocity && !ShooterCharacter->IsPlayingWalkAnimTimeline())
		{
			OnCharacterAnimInstanceWalk.Broadcast();
		}
	}
	else if (ShooterCharacter->IsPlayingIdleAnimTimeline() || ShooterCharacter->IsPlayingWalkAnimTimeline())
	{
		OnCharacterAnimInstanceThirdOrCombatAction.Broadcast();
	}*/

}

void UCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (ShooterCharacter == nullptr)
	{
		return;
	}

	if (HandsMesh && !bIsThirdAction)
	{
		BendGoalLeftTransform = HandsMesh->GetSocketTransform(BEND_GOAL_LEFT_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		BendGoalRightTransform = HandsMesh->GetSocketTransform(BEND_GOAL_RIGHT_SOCKET_NAME, ERelativeTransformSpace::RTS_World);

		LPalmTransform = HandsMesh->GetSocketTransform(BASE_HUMAN_L_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		RPalmTransform = HandsMesh->GetSocketTransform(BASE_HUMAN_R_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
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
