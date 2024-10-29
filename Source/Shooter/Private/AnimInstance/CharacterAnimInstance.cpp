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
	MovementInputVector = ShooterCharacter->GetMovementInputVector();
	bIsMoveInput = ShooterCharacter->IsMoveInput();
	bIsMoveInputForward = ShooterCharacter->IsMoveInputForward();
	TurnDirection = ShooterCharacter->GetTurnDirection(AO_Yaw);
	LeanDirection = ShooterCharacter->GetLeanDirection();
	bIsToggleSlow = ShooterCharacter->GetIsToggleSlow();
	bIsToggleSprint = ShooterCharacter->GetIsToggleSprint();
	LeanTransitionDuration = ShooterCharacter->GetLeanTransitionDuration();
	AnimationTransitionDuration = ShooterCharacter->GetDefaultAnimationTransitionDuration();
	CurrentStance = ShooterCharacter->GetCurrentStance();

	bIsAccelerating = ShooterCharacter->GetCharacterMovement() && ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0;
	
	FVector CharacterVelocity = ShooterCharacter->GetVelocity();
	CharacterVelocity.Z = 0.0;
	Speed = CharacterVelocity.Size();

	float VelocityYawInWorld = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity()).Yaw;
	MovementDirection = UKismetMathLibrary::NormalizedDeltaRotator(FRotator(0.0, VelocityYawInWorld, 0.0), FRotator(0.0, ShooterCharacter->GetActorRotation().Yaw, 0.0)).Yaw;
	/*UE_LOG(LogTemp, Warning, TEXT("MovementDirection: %f"), MovementDirection);*/

	if (bIsAccelerating)
	{
		UE_LOG(LogTemp, Warning, TEXT("bIsAccelerating"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Not bIsAccelerating"));
	}
	UE_LOG(LogTemp, Warning, TEXT("Speed: %f"), Speed);

	
	//bool bIsYawExceedingMaxLimig = AO_Yaw < -90.0f || AO_Yaw > 90.0f;
	if (TurnDirection != ETurnDirection::TD_None)
	{
		float YawExceedingMaxLimit = 0.0f;
		if (AO_Yaw < -90.0f)		YawExceedingMaxLimit = AO_Yaw + 90.0f;
		else if (AO_Yaw > 90.0f)	YawExceedingMaxLimit = AO_Yaw - 90.0f;

		if (YawExceedingMaxLimit != 0.0f)
		{
			ShooterCharacter->AddActorLocalRotation(FRotator(0.0, YawExceedingMaxLimit, 0.0));
			AO_Yaw -= YawExceedingMaxLimit;
		}
	}
	//else if (bIsMoveInput && AO_Yaw != 0.0f)
	else if (bIsMoveInput)
	{
		float YawRotationStep = ShooterCharacter->GetCharacterMovement()->RotationRate.Yaw * DeltaSeconds;
		if (FMath::Abs(AO_Yaw) < YawRotationStep)
		{
			YawRotationStep = AO_Yaw;
		}
		else if (AO_Yaw < 0.0)
		{
			YawRotationStep *= -1.0f;
		}
		
		ShooterCharacter->AddActorLocalRotation(FRotator(0.0, YawRotationStep, 0.0));
		AO_Yaw -= YawRotationStep;
	}

	//ShooterCharacter->bUseControllerRotationYaw = TurnDirection == ETurnDirection::TD_None && bIsMoveInput && AO_Yaw == 0.0f;
	ShooterCharacter->bUseControllerRotationYaw = true;

	// To Test network latency
	/*FVector ActorLocation = ShooterCharacter->GetActorLocation();
	if (ShooterCharacter->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("HasAuthority: ActorLocation: X=%f, Y=%f, Z=%f"), ActorLocation.X, ActorLocation.Y, ActorLocation.Z);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("!HasAuthority: ActorLocation: X=%f, Y=%f, Z=%f"), ActorLocation.X, ActorLocation.Y, ActorLocation.Z);
	}*/
	
	//if (bIsMoveInput)	//TurnDirection != ETurnDirection::TD_None || 
	//{
	//	float ExceedingYaw = 0.0f;
	//	if (AO_Yaw < -90.0f)
	//	{
	//		ExceedingYaw = AO_Yaw + 90.0f;
	//	}
	//	else if (AO_Yaw > 90.0f)
	//	{
	//		ExceedingYaw = AO_Yaw - 90.0f;
	//	}

	//	float YawRotationStep = ShooterCharacter->GetCharacterMovement()->RotationRate.Yaw * DeltaSeconds;
	//	if (FMath::Abs(AO_Yaw) < YawRotationStep)
	//	{
	//		YawRotationStep = AO_Yaw;
	//	}
	//	else if (AO_Yaw < 0.0)
	//	{
	//		YawRotationStep *= -1.0f;
	//	}
	//	ExceedingYaw += YawRotationStep;

	//	ShooterCharacter->AddActorLocalRotation(FRotator(0.0, ExceedingYaw, 0.0));
	//	AO_Yaw -= ExceedingYaw;
	//}

	//ShooterCharacter->bUseControllerRotationYaw = TurnDirection == ETurnDirection::TD_None && bIsMoveInput && FMath::IsNearlyZero(AO_Yaw, UE_KINDA_SMALL_NUMBER);

}
