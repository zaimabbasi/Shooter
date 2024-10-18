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
	
	USkeletalMeshComponent* CharacterMesh = ShooterCharacter->GetMesh();
	HandsMesh = ShooterCharacter->GetHandsMesh();
	if (CharacterMesh && HandsMesh)
	{
		LCollarboneTransform = HandsMesh->GetSocketTransform(BASE_HUMAN_L_COLLARBONE_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		LCollarboneTransform = LCollarboneTransform.GetRelativeTransform(CharacterMesh->GetSocketTransform(L_COLLARBONE_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World));
		RCollarboneTransform = HandsMesh->GetSocketTransform(BASE_HUMAN_R_COLLARBONE_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		RCollarboneTransform = RCollarboneTransform.GetRelativeTransform(CharacterMesh->GetSocketTransform(R_COLLARBONE_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World));
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

	AnimationTransitionDuration = ShooterCharacter->GetDefaultAnimationTransitionDuration();

	CurrentStance = ShooterCharacter->GetCurrentStance();

}
