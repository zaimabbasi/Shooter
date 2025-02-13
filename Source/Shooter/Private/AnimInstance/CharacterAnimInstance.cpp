// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/CharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Actor/Weapon.h"
#include "ActorComponent/CombatComponent.h"
#include "Character/ShooterCharacter.h"
#include "Enum/TurnDirection.h"
#include "Struct/ShooterUtility.h"
#include "Type/ShooterNameType.h"

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
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
	AO_Yaw = ShooterCharacter->GetAO_Yaw(AO_Yaw, DeltaSeconds);
	AO_Pitch = ShooterCharacter->GetAO_Pitch(AO_Pitch, DeltaSeconds);
	bIsAccelerating = ShooterCharacter->GetCurrentAcceleration().SizeSquared2D() > 0.0f;
	TurnDirection = ShooterCharacter->GetTurnDirection(AO_Yaw);
	LeanDirection = ShooterCharacter->GetLeanDirection();
	bIsCrouched = ShooterCharacter->bIsCrouched;
	bIsProned = ShooterCharacter->bIsProned;
	bIsSlowing = ShooterCharacter->bIsSlowing;
	bIsSprinting = ShooterCharacter->bIsSprinting;
	LeanTransitionDuration = ShooterCharacter->GetLeanTransitionDuration();
	AnimationTransitionDuration = ShooterCharacter->GetDefaultAnimationTransitionDuration();

	FVector Velocity = ShooterCharacter->GetVelocity();
	bHasVelocity = Velocity.SizeSquared2D() > 0.0f;

	if (bHasVelocity)
	{
		float VelocityYaw = UKismetMathLibrary::MakeRotFromX(Velocity).Yaw;
		float ActorRotationYaw = ShooterCharacter->GetActorRotation().Yaw;
		VelocityYawOffset = UKismetMathLibrary::NormalizedDeltaRotator(FRotator(0.0, VelocityYaw, 0.0), FRotator(0.0, ActorRotationYaw, 0.0)).Yaw;
	}

	bool bIsTransition = ShooterCharacter->GetIsTransition();
	bIsThirdAction = bIsSprinting || bIsTransition || (bIsProned && bHasVelocity);

	UCombatComponent* CombatComponent = ShooterCharacter->GetCombatComponent();
	AWeapon* EquippedWeapon = CombatComponent ? CombatComponent->GetEquippedWeapon() : nullptr;
	bIsWeaponEquipped = EquippedWeapon != nullptr;
	bIsEquippedWeaponPistol = EquippedWeapon && EquippedWeapon->IsPistol();
	bIsEquippedWeaponOneHanded = EquippedWeapon && EquippedWeapon->GetIsOneHanded();

	UCharacterMovementComponent* MovementComponent = ShooterCharacter->GetCharacterMovement();
	float bUseControllerDesiredRotation = MovementComponent && MovementComponent->bUseControllerDesiredRotation;

	if (!bUseControllerDesiredRotation && (bHasVelocity || TurnDirection != ETurnDirection::TD_None))
	{
		OnCharacterAnimInstanceControllerDesiredRotationNeeded.Broadcast(true);
	}
	else if (bUseControllerDesiredRotation && !bHasVelocity && TurnDirection == ETurnDirection::TD_None)
	{
		OnCharacterAnimInstanceControllerDesiredRotationNeeded.Broadcast(false);
	}

	float YawExceedingMaxLimit = ShooterCharacter->GetYawExceedingMaxLimit(AO_Yaw);
	if (!FMath::IsNearlyZero(YawExceedingMaxLimit))
	{
		ShooterCharacter->AddActorLocalRotation(FRotator(0.0, YawExceedingMaxLimit, 0.0));
		AO_Yaw -= YawExceedingMaxLimit;
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

void UCharacterAnimInstance::AnimNotify_TurnInPlace() const
{
	OnCharacterAnimInstanceTurnInPlace.Broadcast();
}
