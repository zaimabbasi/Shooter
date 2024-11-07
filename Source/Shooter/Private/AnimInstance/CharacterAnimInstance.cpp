// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/CharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
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
	
	USkeletalMeshComponent* CharacterMesh = ShooterCharacter->GetMesh();
	HandsMesh = ShooterCharacter->GetHandsMesh();
	if (CharacterMesh && HandsMesh)
	{
		LCollarboneTransform = HandsMesh->GetSocketTransform(BASE_HUMAN_L_COLLARBONE_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		LCollarboneTransform = LCollarboneTransform.GetRelativeTransform(CharacterMesh->GetSocketTransform(L_COLLARBONE_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World));
		RCollarboneTransform = HandsMesh->GetSocketTransform(BASE_HUMAN_R_COLLARBONE_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		RCollarboneTransform = RCollarboneTransform.GetRelativeTransform(CharacterMesh->GetSocketTransform(R_COLLARBONE_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World));
	}

	AO_Yaw = ShooterCharacter->GetAO_Yaw(AO_Yaw, DeltaSeconds);
	AO_Pitch = ShooterCharacter->GetAO_Pitch(AO_Pitch, DeltaSeconds);
	bIsAccelerating = ShooterCharacter->GetCurrentAcceleration().SizeSquared2D() > 0.0f;
	bHasVelocity = ShooterCharacter->GetVelocity().SizeSquared2D() > 0.0f;
	TurnDirection = ShooterCharacter->GetTurnDirection(AO_Yaw);
	LeanDirection = ShooterCharacter->GetLeanDirection();
	bIsToggleSlow = ShooterCharacter->GetIsToggleSlow();
	bIsToggleSprint = ShooterCharacter->GetIsToggleSprint();
	LeanTransitionDuration = ShooterCharacter->GetLeanTransitionDuration();
	AnimationTransitionDuration = ShooterCharacter->GetDefaultAnimationTransitionDuration();
	CurrentStance = ShooterCharacter->GetCurrentStance();
	
	if (bHasVelocity)
	{
		float VelocityYaw = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity()).Yaw;
		VelocityYawOffset = UKismetMathLibrary::NormalizedDeltaRotator(FRotator(0.0, VelocityYaw, 0.0), FRotator(0.0, ShooterCharacter->GetActorRotation().Yaw, 0.0)).Yaw;
	}
	if (bIsAccelerating)
	{
		float AccelerationYaw = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetCurrentAcceleration()).Yaw;
		AccelerationYawOffset = UKismetMathLibrary::NormalizedDeltaRotator(FRotator(0.0, AccelerationYaw, 0.0), FRotator(0.0, ShooterCharacter->GetActorRotation().Yaw, 0.0)).Yaw;
	}
	
	float bUseControllerDesiredRotation = ShooterCharacter->GetUseControllerDesiredRotation();
	if (!bUseControllerDesiredRotation && (bHasVelocity || TurnDirection != ETurnDirection::TD_None))
	{
		OnCharacterAnimInstanceControllerDesiredRotationNeeded.Broadcast(true);
	}
	else if (bUseControllerDesiredRotation && !bHasVelocity && TurnDirection == ETurnDirection::TD_None)
	{
		OnCharacterAnimInstanceControllerDesiredRotationNeeded.Broadcast(false);
	}

	if (float YawExceedingMaxLimit = ShooterCharacter->GetYawExceedingMaxLimit(AO_Yaw))
	{
		ShooterCharacter->AddActorLocalRotation(FRotator(0.0, YawExceedingMaxLimit, 0.0));
		AO_Yaw -= YawExceedingMaxLimit;
	}

}

void UCharacterAnimInstance::AnimNotify_TurnInPlace() const
{
	OnCharacterAnimInstanceTurnInPlace.Broadcast();
}
