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
	if (ShooterCharacter && ShooterCharacter->GetCharacterMovement())
	{
		CharacterMovementRotationRateYaw = ShooterCharacter->GetCharacterMovement()->RotationRate.Yaw;
	}
	TurnInPlaceRotationRateYaw = 180.0f;

}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
		if (ShooterCharacter && ShooterCharacter->GetCharacterMovement())
		{
			CharacterMovementRotationRateYaw = ShooterCharacter->GetCharacterMovement()->RotationRate.Yaw;
		}
	}
	if (ShooterCharacter == nullptr)
	{
		return;
	}
	
	HandsMesh = ShooterCharacter->GetHandsMesh();
	AO_Yaw = ShooterCharacter->GetAO_Yaw(AO_Yaw, DeltaSeconds);
	AO_Pitch = ShooterCharacter->GetAO_Pitch(AO_Pitch, DeltaSeconds);
	LeaningDirection = ShooterCharacter->GetLeaningDirection();
	TurningDirection = ShooterCharacter->GetTurningDirection();
	bIsCrouched = ShooterCharacter->bIsCrouched;
	bIsProned = ShooterCharacter->bIsProned;
	bIsSlowing = ShooterCharacter->bIsSlowing;
	bIsSprinting = ShooterCharacter->bIsSprinting;
	LeaningTransitionDuration = ShooterCharacter->GetLeaningTransitionDuration();
	bIsAccelerating = IsAccelerating();
	bHasVelocity = HasVelocity();
	bIsThirdAction = IsThirdAction();

	UCharacterCombatComponent* CharacterCombatComponent = ShooterCharacter->GetCharacterCombatComponent();
	AWeapon* EquippedWeapon = CharacterCombatComponent ? CharacterCombatComponent->GetEquippedWeapon() : nullptr;
	bIsWeaponEquipped = EquippedWeapon != nullptr;
	bIsEquippedWeaponPistol = EquippedWeapon && EquippedWeapon->IsPistol();
	bIsEquippedWeaponOneHanded = EquippedWeapon && EquippedWeapon->GetIsOneHanded();

	if (TurningDirection != ETurningDirection::TD_None)
	{
		float AllowedYaw = AllowedAO_Yaw();
		float ExceedingYaw = AO_Yaw < -AllowedYaw ? AO_Yaw + AllowedYaw : AO_Yaw > AllowedYaw ? AO_Yaw - AllowedYaw : 0.0f;
		if (!FMath::IsNearlyZero(ExceedingYaw))
		{
			ShooterCharacter->SetActorRotation(FRotator(0.0f, ShooterCharacter->GetActorRotation().Yaw + ExceedingYaw, 0.0f));
		}
	}

}

void UCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (ShooterCharacter == nullptr)
	{
		return;
	}

	if (bHasVelocity)
	{
		float VelocityYaw = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity()).Yaw;
		VelocityYawOffset = UKismetMathLibrary::NormalizedDeltaRotator(FRotator(0.0, VelocityYaw, 0.0), FRotator(0.0, ShooterCharacter->GetActorRotation().Yaw, 0.0)).Yaw;
	}

	if (HandsMesh && !bIsThirdAction)
	{
		BendGoalLeftTransform = HandsMesh->GetSocketTransform(BEND_GOAL_LEFT_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		BendGoalRightTransform = HandsMesh->GetSocketTransform(BEND_GOAL_RIGHT_SOCKET_NAME, ERelativeTransformSpace::RTS_World);

		LPalmTransform = HandsMesh->GetSocketTransform(BASE_HUMAN_L_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		RPalmTransform = HandsMesh->GetSocketTransform(BASE_HUMAN_R_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
	}

	UCharacterMovementComponent* MovementComponent = ShooterCharacter->GetCharacterMovement();
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

		if (FMath::Abs(AO_Yaw) > AllowedAO_Yaw())
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

float UCharacterAnimInstance::AllowedAO_Yaw() const
{
	if (ShooterCharacter == nullptr)
	{
		return 0.0f;
	}
	return (!ShooterCharacter->bIsCrouched && !ShooterCharacter->bIsProned) || ShooterCharacter->bIsCrouched ? 90.0f : 45.0f;
}

bool UCharacterAnimInstance::IsAccelerating() const
{
	return ShooterCharacter && ShooterCharacter->GetCurrentAcceleration().SizeSquared2D() > 0.0f;
}

bool UCharacterAnimInstance::IsThirdAction() const
{
	return ShooterCharacter && (ShooterCharacter->bIsSprinting || ShooterCharacter->GetIsTransition() || (ShooterCharacter->bIsProned && HasVelocity()));
}

bool UCharacterAnimInstance::HasVelocity() const
{
	return ShooterCharacter && ShooterCharacter->GetVelocity().SizeSquared2D() > 0.0f;
}
