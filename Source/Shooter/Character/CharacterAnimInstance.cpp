// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
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
		FVector LPalmOutLocation;
		FRotator LPalmOutRotation;
		FVector RPalmOutLocation;
		FRotator RPalmOutRotation;
		LPalmTransform = HandsMesh->GetSocketTransform(TEXT("Base-HumanLPalm"), ERelativeTransformSpace::RTS_World);
		RPalmTransform = HandsMesh->GetSocketTransform(TEXT("Base-HumanRPalm"), ERelativeTransformSpace::RTS_World);
		CharacterMesh->TransformToBoneSpace(TEXT("IK_S_LPalm"), LPalmTransform.GetLocation(), FRotator(LPalmTransform.GetRotation()), LPalmOutLocation, LPalmOutRotation);
		CharacterMesh->TransformToBoneSpace(TEXT("IK_S_RPalm"), RPalmTransform.GetLocation(), FRotator(RPalmTransform.GetRotation()), RPalmOutLocation, RPalmOutRotation);
		LPalmTransform.SetLocation(LPalmOutLocation);
		LPalmTransform.SetRotation(FQuat(LPalmOutRotation));
		RPalmTransform.SetLocation(RPalmOutLocation);
		RPalmTransform.SetRotation(FQuat(RPalmOutRotation));
	}

	AO_Yaw = ShooterCharacter->GetAO_Yaw();
	AO_Pitch = ShooterCharacter->GetAO_Pitch();

	MovementInputVector = ShooterCharacter->GetMovementInputVector();

	bIsMoving = ShooterCharacter->IsMoving();

	bIsCrouched = ShooterCharacter->bIsCrouched;

	TurnInPlace = ShooterCharacter->GetTurnInPlace();

	bIsSlow = ShooterCharacter->GetIsSlow();

}
