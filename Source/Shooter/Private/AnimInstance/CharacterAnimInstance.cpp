// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/CharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/ShooterCharacter.h"
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
	if (HandsMesh)
	{
		BaseHumanRibcageTransform = HandsMesh->GetSocketTransform(BASE_HUMAN_RIBCAGE_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
	}
	// No need for now
	/*USkeletalMeshComponent* CharacterMesh = ShooterCharacter->GetMesh();
	if (CharacterMesh && HandsMesh)
	{
		LPalmTransform = HandsMesh->GetSocketTransform(BASE_HUMAN_L_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		LPalmTransform = FShooterUtility::TransformToBoneSpace(CharacterMesh, IK_S_L_PALM_SOCKET_NAME, LPalmTransform);
		RPalmTransform = HandsMesh->GetSocketTransform(BASE_HUMAN_R_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		RPalmTransform = FShooterUtility::TransformToBoneSpace(CharacterMesh, IK_S_R_PALM_SOCKET_NAME, RPalmTransform);
	}*/

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

	AnimationTransitionDuration = ShooterCharacter->GetDefaultAnimationTransitionDuration();

	CurrentStance = ShooterCharacter->GetCurrentStance();

}
