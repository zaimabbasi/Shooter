// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Shooter/Utility/ShooterUtility.h"
#include "ShooterCharacter.h"

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
		LPalmTransform = HandsMesh->GetSocketTransform(TEXT("Base-HumanLPalm"), ERelativeTransformSpace::RTS_World);
		LPalmTransform = FShooterUtility::TransformToBoneSpace(CharacterMesh, TEXT("IK_S_LPalm"), LPalmTransform);
		RPalmTransform = HandsMesh->GetSocketTransform(TEXT("Base-HumanRPalm"), ERelativeTransformSpace::RTS_World);
		RPalmTransform = FShooterUtility::TransformToBoneSpace(CharacterMesh, TEXT("IK_S_RPalm"), RPalmTransform);
	}

	AO_Yaw = ShooterCharacter->GetAO_Yaw();
	AO_Pitch = ShooterCharacter->GetAO_Pitch();

	MovementInputVector = ShooterCharacter->GetMovementInputVector();
	bIsMoveInput = MovementInputVector.Size() > 0.0 ? true : false;
	bIsMoveInputForward = MovementInputVector.Y == 1.0 ? true : false;

	TurnDirection = ShooterCharacter->GetTurnDirection();

	bIsToggleSlow = ShooterCharacter->GetIsToggleSlow();

	bIsToggleSprint = ShooterCharacter->GetIsToggleSprint();

	LeanDirection = ShooterCharacter->GetLeanDirection();

	LeanTransitionDuration = ShooterCharacter->GetLeanTransitionDuration();

	AnimationTransitionDuration = ShooterCharacter->DefaultAnimationTransitionDuration;

	CurrentStance = ShooterCharacter->GetCurrentStance();

}
