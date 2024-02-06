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

	FVector Velocity = ShooterCharacter->GetVelocity();
	Velocity.Z = 0.0;
	Speed = Velocity.Size();

	bIsAccelerating = ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0 ? true : false;

	FRotator BaseAimRotation = ShooterCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, BaseAimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaSeconds, 10.0f);
	YawOffset = DeltaRotation.Yaw;

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

	AO_Pitch = ShooterCharacter->GetAO_Pitch();

}
