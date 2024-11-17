// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterMovementComponent/ShooterCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

FNetworkPredictionData_Client_ShooterCharacter::FNetworkPredictionData_Client_ShooterCharacter(const UCharacterMovementComponent& ClientMovement) :
	Super(ClientMovement)
{
}

FSavedMovePtr FNetworkPredictionData_Client_ShooterCharacter::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_ShooterCharacter());
}

bool FSavedMove_ShooterCharacter::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	const FSavedMove_ShooterCharacter* NewSavedMove_ShooterCharacter = StaticCast<FSavedMove_ShooterCharacter*>(NewMove.Get());
	if (bWantsToSprint != NewSavedMove_ShooterCharacter->bWantsToSprint)
	{
		return false;
	}

	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void FSavedMove_ShooterCharacter::Clear()
{
	Super::Clear();

	bWantsToSprint = 0;
}

uint8 FSavedMove_ShooterCharacter::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (bWantsToSprint)
	{
		Result |= FLAG_Custom_0;
	}

	return Result;
}

void FSavedMove_ShooterCharacter::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	UShooterCharacterMovementComponent* ShooterCharacterMovementComponent = Cast<UShooterCharacterMovementComponent>(C->GetCharacterMovement());
	ShooterCharacterMovementComponent->bWantsToSprint = bWantsToSprint;
}

void FSavedMove_ShooterCharacter::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	UShooterCharacterMovementComponent* ShooterCharacterMovementComponent = Cast<UShooterCharacterMovementComponent>(C->GetCharacterMovement());
	bWantsToSprint = ShooterCharacterMovementComponent->bWantsToSprint;
}

UShooterCharacterMovementComponent::UShooterCharacterMovementComponent() :
	bWantsToSprint(false),
	MaxWalkSpeedWalk(250.0f),
	MaxWalkSpeedSprint(500.0f)
{
}

bool UShooterCharacterMovementComponent::CanSprintInCurrentState() const
{
	check(PawnOwner != nullptr);

	float AccelerationYaw = UKismetMathLibrary::MakeRotFromX(Acceleration).Yaw;
	float AccelerationYawOffset = UKismetMathLibrary::NormalizedDeltaRotator(FRotator(0.0, AccelerationYaw, 0.0), FRotator(0.0, PawnOwner->GetActorRotation().Yaw, 0.0)).Yaw;

	return (AccelerationYawOffset > -90.0f && AccelerationYawOffset < 90.0f);
}

FNetworkPredictionData_Client* UShooterCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		UShooterCharacterMovementComponent* MutableThis = const_cast<UShooterCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_ShooterCharacter(*this);
		
		// Probably remove these comments
		/*MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.0f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.0f;*/
		/*MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = UCharacterMovementComponent::NetworkMaxSmoothUpdateDistance;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = UCharacterMovementComponent::NetworkNoSmoothUpdateDistance;*/
	}

	return ClientPredictionData;
}

void UShooterCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	if (IsMovingOnGround())
	{
		if (MaxWalkSpeed == MaxWalkSpeedSprint && (!bWantsToSprint || !CanSprintInCurrentState()))
		{
			MaxWalkSpeed = MaxWalkSpeedWalk;
		}
		else if (MaxWalkSpeed == MaxWalkSpeedWalk && bWantsToSprint && CanSprintInCurrentState())
		{
			MaxWalkSpeed = MaxWalkSpeedSprint;
		}
	}
}

void UShooterCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	bWantsToSprint = ((Flags & FSavedMove_Character::FLAG_Custom_0) != 0);
}
