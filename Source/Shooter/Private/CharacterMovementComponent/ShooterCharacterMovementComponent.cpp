// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterMovementComponent/ShooterCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/ShooterCharacter.h"

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
	if (bWantsToProne != NewSavedMove_ShooterCharacter->bWantsToProne)
	{
		return false;
	}
	if (bWantsToSlow != NewSavedMove_ShooterCharacter->bWantsToSlow)
	{
		return false;
	}
	if (bWantsToSprint != NewSavedMove_ShooterCharacter->bWantsToSprint)
	{
		return false;
	}

	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void FSavedMove_ShooterCharacter::Clear()
{
	Super::Clear();

	bWantsToProne = false;
	bWantsToSlow = false;
	bWantsToSprint = false;
}

uint8 FSavedMove_ShooterCharacter::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (bWantsToProne)
	{
		Result |= FLAG_Custom_0;
	}
	if (bWantsToSlow)
	{
		Result |= FLAG_Custom_1;
	}
	if (bWantsToSprint)
	{
		Result |= FLAG_Custom_2;
	}

	return Result;
}

void FSavedMove_ShooterCharacter::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	UShooterCharacterMovementComponent* ShooterCharacterMovementComponent = Cast<UShooterCharacterMovementComponent>(C->GetCharacterMovement());
	ShooterCharacterMovementComponent->bWantsToProne = bWantsToProne;
	ShooterCharacterMovementComponent->bWantsToSlow = bWantsToSlow;
	ShooterCharacterMovementComponent->bWantsToSprint = bWantsToSprint;
}

void FSavedMove_ShooterCharacter::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	UShooterCharacterMovementComponent* ShooterCharacterMovementComponent = Cast<UShooterCharacterMovementComponent>(C->GetCharacterMovement());
	bWantsToProne = ShooterCharacterMovementComponent->bWantsToProne;
	bWantsToSlow = ShooterCharacterMovementComponent->bWantsToSlow;
	bWantsToSprint = ShooterCharacterMovementComponent->bWantsToSprint;
}

UShooterCharacterMovementComponent::UShooterCharacterMovementComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	bWantsToProne = false;
	bWantsToSlow = false;
	bWantsToSprint = false;

	MaxWalkSpeedProned = 50.0f;
	MaxWalkSpeedSlow = 168.0f;
	MaxWalkSpeedCrouchedSlow = 46.0f;
	MaxWalkSpeedSprint = 500.0f;
	MaxWalkSpeed = 250.0f;
	MaxWalkSpeedCrouched = 160.0f;

	SetCrouchedHalfHeight(68.0f);
	SetPronedHalfHeight(30.0f);
}

bool UShooterCharacterMovementComponent::CanAttemptJump() const
{
	return Super::CanAttemptJump() && !bWantsToProne;
}

bool UShooterCharacterMovementComponent::CanProneInCurrentState() const
{
	return (IsFalling() || IsMovingOnGround()) && UpdatedComponent && !UpdatedComponent->IsSimulatingPhysics();
}

bool UShooterCharacterMovementComponent::CanSlowInCurrentState() const
{
	return ((MovementMode == MOVE_Walking || MovementMode == MOVE_NavWalking) && !IsProning());
}

bool UShooterCharacterMovementComponent::CanSprintInCurrentState() const
{
	if (!IsValid(CharacterOwner) || !(MovementMode == MOVE_Walking || MovementMode == MOVE_NavWalking) || FMath::IsNearlyZero(Acceleration.SizeSquared2D()))
	{
		return false;
	}
	
	float AccelerationYaw = UKismetMathLibrary::MakeRotFromX(Acceleration).Yaw;
	float AccelerationYawOffset = UKismetMathLibrary::NormalizedDeltaRotator(FRotator(0.0, AccelerationYaw, 0.0), FRotator(0.0, CharacterOwner->GetActorRotation().Yaw, 0.0)).Yaw;

	return (AccelerationYawOffset > -60.0f && AccelerationYawOffset < 60.0f);
}

bool UShooterCharacterMovementComponent::CanWalkOffLedges() const
{
	if (!bCanWalkOffLedgesWhenProning && IsProning())
	{
		return false;
	}

	return Super::CanWalkOffLedges();
}

float UShooterCharacterMovementComponent::GetMaxSpeed() const
{
	switch (MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
		if (!IsCrouching() && !IsProning())
		{
			return IsSprinting() ? MaxWalkSpeedSprint : IsSlowing() ? MaxWalkSpeedSlow : MaxWalkSpeed;
		}
		else if (IsCrouching())
		{
			return IsSlowing() ? MaxWalkSpeedCrouchedSlow : MaxWalkSpeedCrouched;
		}
		else
		{
			return MaxWalkSpeedProned;
		}
	case MOVE_Falling:
		return MaxWalkSpeed;
	case MOVE_Swimming:
		return MaxSwimSpeed;
	case MOVE_Flying:
		return MaxFlySpeed;
	case MOVE_Custom:
		return MaxCustomMovementSpeed;
	case MOVE_None:
	default:
		return 0.f;
	}
}

FNetworkPredictionData_Client* UShooterCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		UShooterCharacterMovementComponent* MutableThis = const_cast<UShooterCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_ShooterCharacter(*this);
	}

	return ClientPredictionData;
}

float UShooterCharacterMovementComponent::GetPronedHalfHeight() const
{
	return PronedHalfHeight;
}

//float UShooterCharacterMovementComponent::GetRemainingProneLockCooldown() const
//{
//	const float CurrentTimestamp = GetTimestamp();
//	const float RemainingCooldown = ProneLockDuration - (CurrentTimestamp - ProneLockTimestamp);
//	return FMath::Clamp(RemainingCooldown, 0.f, ProneLockDuration);
//}

//float UShooterCharacterMovementComponent::GetTimestamp() const
//{
//	if (CharacterOwner->GetLocalRole() == ROLE_Authority)
//	{
//		if (CharacterOwner->IsLocallyControlled())
//		{
//			// Server owned character
//			return GetWorld()->GetTimeSeconds();
//		}
//		else
//		{
//			// Server remote character
//			const FNetworkPredictionData_Server_Character* ServerData = GetPredictionData_Server_Character();
//			return ServerData->CurrentClientTimeStamp;
//		}
//	}
//	else
//	{
//		// Client owned character
//		const FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
//		return ClientData->CurrentTimeStamp;
//	}
//}

bool UShooterCharacterMovementComponent::HasValidData() const
{
	return Super::HasValidData() && IsValid(ShooterCharacterOwner);
}

//bool UShooterCharacterMovementComponent::IsProneLocked() const
//{
//	if (CharacterOwner && CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)
//	{
//		// Sim proxies don't prone lock
//		return false;
//	}
//	return bProneLocked;
//}

bool UShooterCharacterMovementComponent::IsProning() const
{
	return ShooterCharacterOwner && ShooterCharacterOwner->bIsProned;
}

//bool UShooterCharacterMovementComponent::IsProneLockOnTimer() const
//{
//	return GetRemainingProneLockCooldown() > 0.f;
//}

bool UShooterCharacterMovementComponent::IsSlowing() const
{
	return ShooterCharacterOwner && ShooterCharacterOwner->bIsSlowing;
}

bool UShooterCharacterMovementComponent::IsSprinting() const
{
	return ShooterCharacterOwner && ShooterCharacterOwner->bIsSprinting;
}

void UShooterCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
}

void UShooterCharacterMovementComponent::PostLoad()
{
	Super::PostLoad();

	ShooterCharacterOwner = Cast<AShooterCharacter>(PawnOwner);
}

void UShooterCharacterMovementComponent::Prone(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	if (!bClientSimulation && !CanProneInCurrentState())
	{
		return;
	}

	// See if collision is already at desired size.
	if (CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == PronedHalfHeight &&
		CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius() == PronedRadius)
	{
		if (!bClientSimulation)
		{
			ShooterCharacterOwner->bIsProned = true;
		}
		ShooterCharacterOwner->OnStartProne(0.f, 0.f);
		//SetProneLock(true);
		return;
	}

	if (bClientSimulation && CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)
	{
		// restore collision size before prone
		const ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
		bShrinkProxyCapsule = true;
	}

	// Change collision size to prone dimensions
	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float OldUnscaledRadius = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();

	// Height is not allowed to be smaller than radius.
	const float ClampedPronedHalfHeight = FMath::Max3(0.f, PronedRadius, PronedHalfHeight);
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(PronedRadius, ClampedPronedHalfHeight);
	float HalfHeightAdjust = (OldUnscaledHalfHeight - ClampedPronedHalfHeight);
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	if (!bClientSimulation)
	{
		// Proned to a larger height? (this is rare)
		if (ClampedPronedHalfHeight > OldUnscaledHalfHeight)
		{
			FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(ProneTrace), false, CharacterOwner);
			FCollisionResponseParams ResponseParam;
			InitCollisionParams(CapsuleParams, ResponseParam);
			const bool bEncroached = GetWorld()->OverlapBlockingTestByChannel(UpdatedComponent->GetComponentLocation() - FVector(0.f, 0.f, ScaledHalfHeightAdjust), FQuat::Identity,
				UpdatedComponent->GetCollisionObjectType(), GetPawnCapsuleCollisionShape(SHRINK_None), CapsuleParams, ResponseParam);

			// If encroached, cancel
			if (bEncroached)
			{
				CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, OldUnscaledHalfHeight);
				return;
			}
		}

		if (bCrouchMaintainsBaseLocation)
		{
			// Intentionally not using MoveUpdatedComponent, where a horizontal plane constraint would prevent the base of the capsule from staying at the same spot.
			UpdatedComponent->MoveComponent(FVector(0.f, 0.f, -ScaledHalfHeightAdjust), UpdatedComponent->GetComponentQuat(), true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
		}

		ShooterCharacterOwner->bIsProned = true;
	}

	// Our capsule is growing during prone, test for encroaching from radius
	FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(ProneTrace), false, CharacterOwner);
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(CapsuleParams, ResponseParam);
	FHitResult Hit;
	const FVector Start = UpdatedComponent->GetComponentLocation() - FVector(0.f, 0.f, ScaledHalfHeightAdjust);
	const FVector End = UpdatedComponent->GetComponentLocation() - FVector(0.f, 0.f, ScaledHalfHeightAdjust * 1.01f);
	if (GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, UpdatedComponent->GetCollisionObjectType(), FCollisionShape::MakeCapsule(PronedRadius, PronedHalfHeight), CapsuleParams, ResponseParam))
	{
		if (Hit.bStartPenetrating)
		{
			HandleImpact(Hit);
			SlideAlongSurface(FVector::DownVector, 1.f, Hit.Normal, Hit, true);

			if (Hit.bStartPenetrating)
			{
				OnCharacterStuckInGeometry(&Hit);
			}
		}
	}

	bForceNextFloorCheck = true;

	//SetProneLock(true);

	// OnStartProne takes the change from the Default size, not the current one (though they are usually the same).
	const float MeshAdjust = ScaledHalfHeightAdjust;
	const ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	HalfHeightAdjust = (DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - ClampedPronedHalfHeight);
	ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	AdjustProxyCapsuleSize();
	ShooterCharacterOwner->OnStartProne(HalfHeightAdjust, ScaledHalfHeightAdjust);

	// Don't smooth this change in mesh position
	if ((bClientSimulation && CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy) || (IsNetMode(NM_ListenServer) && CharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy))
	{
		FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
		if (ClientData)
		{
			ClientData->MeshTranslationOffset -= FVector(0.f, 0.f, MeshAdjust);
			ClientData->OriginalMeshTranslationOffset = ClientData->MeshTranslationOffset;
		}
	}
}

void UShooterCharacterMovementComponent::SetPronedHalfHeight(const float NewValue)
{
	PronedHalfHeight = NewValue;
	PronedRadius = NewValue;

	if (ShooterCharacterOwner != nullptr)
	{
		ShooterCharacterOwner->RecalculatePronedEyeHeight();
	}
}

//void UShooterCharacterMovementComponent::SetProneLock(bool bLock)
//{
//	if (bLock)
//	{
//		ProneLockTimestamp = GetTimestamp();
//	}
//	bProneLocked = bLock;
//}

void UShooterCharacterMovementComponent::SetUpdatedComponent(USceneComponent* NewUpdatedComponent)
{
	Super::SetUpdatedComponent(NewUpdatedComponent);

	ShooterCharacterOwner = Cast<AShooterCharacter>(PawnOwner);
}

void UShooterCharacterMovementComponent::Slow()
{
	ShooterCharacterOwner->bIsSlowing = true;
}

void UShooterCharacterMovementComponent::Sprint()
{
	ShooterCharacterOwner->bIsSprinting = true;
}

void UShooterCharacterMovementComponent::UnProne(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	/*if (IsProneLocked())
	{
		return;
	}*/

	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();

	// See if collision is already at desired size.
	if (CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() &&
		CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius() == DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius())
	{
		if (!bClientSimulation)
		{
			ShooterCharacterOwner->bIsProned = false;
		}
		ShooterCharacterOwner->OnEndProne(0.f, 0.f);
		return;
	}

	const float CurrentPronedHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float HalfHeightAdjust = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - OldUnscaledHalfHeight;
	const float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();

	// Grow to unproned size.
	check(CharacterOwner->GetCapsuleComponent());

	if (!bClientSimulation)
	{
		// Try to stay in place and see if the larger capsule fits. We use a slightly taller capsule to avoid penetration.
		const UWorld* MyWorld = GetWorld();
		constexpr float SweepInflation = UE_KINDA_SMALL_NUMBER * 10.f;
		FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(ProneTrace), false, CharacterOwner);
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(CapsuleParams, ResponseParam);

		// Compensate for the difference between current capsule size and standing size
		const FCollisionShape StandingCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, -SweepInflation - ScaledHalfHeightAdjust); // Shrink by negative amount, so actually grow it.
		const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
		bool bEncroached = true;

		if (!bCrouchMaintainsBaseLocation)
		{
			// Expand in place
			bEncroached = MyWorld->OverlapBlockingTestByChannel(PawnLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

			if (bEncroached)
			{
				// Try adjusting capsule position to see if we can avoid encroachment.
				if (ScaledHalfHeightAdjust > 0.f)
				{
					// Shrink to a short capsule, sweep down to base to find where that would hit something, and then try to stand up from there.
					float PawnRadius, PawnHalfHeight;
					CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);
					const float ShrinkHalfHeight = PawnHalfHeight - PawnRadius;
					const float TraceDist = PawnHalfHeight - ShrinkHalfHeight;
					const FVector Down = FVector(0.f, 0.f, -TraceDist);

					FHitResult Hit(1.f);
					const FCollisionShape ShortCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, ShrinkHalfHeight);
					MyWorld->SweepSingleByChannel(Hit, PawnLocation, PawnLocation + Down, FQuat::Identity, CollisionChannel, ShortCapsuleShape, CapsuleParams);
					if (Hit.bStartPenetrating)
					{
						bEncroached = true;
					}
					else
					{
						// Compute where the base of the sweep ended up, and see if we can stand there
						const float DistanceToBase = (Hit.Time * TraceDist) + ShortCapsuleShape.Capsule.HalfHeight;
						const FVector NewLoc = FVector(PawnLocation.X, PawnLocation.Y, PawnLocation.Z - DistanceToBase + StandingCapsuleShape.Capsule.HalfHeight + SweepInflation + MIN_FLOOR_DIST / 2.f);
						bEncroached = MyWorld->OverlapBlockingTestByChannel(NewLoc, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
						if (!bEncroached)
						{
							// Intentionally not using MoveUpdatedComponent, where a horizontal plane constraint would prevent the base of the capsule from staying at the same spot.
							UpdatedComponent->MoveComponent(NewLoc - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
						}
					}
				}
			}
		}
		else
		{
			// Expand while keeping base location the same.
			FVector StandingLocation = PawnLocation + FVector(0.f, 0.f, StandingCapsuleShape.GetCapsuleHalfHeight() - CurrentPronedHalfHeight);
			bEncroached = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

			if (bEncroached)
			{
				if (IsMovingOnGround())
				{
					// Something might be just barely overhead, try moving down closer to the floor to avoid it.
					constexpr float MinFloorDist = UE_KINDA_SMALL_NUMBER * 10.f;
					if (CurrentFloor.bBlockingHit && CurrentFloor.FloorDist > MinFloorDist)
					{
						StandingLocation.Z -= CurrentFloor.FloorDist - MinFloorDist;
						bEncroached = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
					}
				}
			}

			if (!bEncroached)
			{
				// Commit the change in location.
				UpdatedComponent->MoveComponent(StandingLocation - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
				bForceNextFloorCheck = true;
			}
		}

		// If still encroached then abort.
		if (bEncroached)
		{
			return;
		}

		ShooterCharacterOwner->bIsProned = false;
	}
	else
	{
		bShrinkProxyCapsule = true;
	}

	// Now call SetCapsuleSize() to cause touch/untouch events and actually grow the capsule
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), true);

	const float MeshAdjust = ScaledHalfHeightAdjust;
	AdjustProxyCapsuleSize();
	ShooterCharacterOwner->OnEndProne(HalfHeightAdjust, ScaledHalfHeightAdjust);

	// Don't smooth this change in mesh position
	if ((bClientSimulation && CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy) || (IsNetMode(NM_ListenServer) && CharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy))
	{
		FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
		if (ClientData)
		{
			ClientData->MeshTranslationOffset += FVector(0.f, 0.f, MeshAdjust);
			ClientData->OriginalMeshTranslationOffset = ClientData->MeshTranslationOffset;
		}
	}
}

void UShooterCharacterMovementComponent::UnSlow()
{
	ShooterCharacterOwner->bIsSlowing = false;
}

void UShooterCharacterMovementComponent::UnSprint()
{
	ShooterCharacterOwner->bIsSprinting = false;
}

void UShooterCharacterMovementComponent::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateAfterMovement(DeltaSeconds);

	// Proxies get replicated prone state.
	if (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		// Unprone if no longer allowed to be proned
		if (IsProning() && !CanProneInCurrentState())
		{
			UnProne(false);
		}

		// Unslow if no longer allowed to be slowing
		if (IsSlowing() && !CanSlowInCurrentState())
		{
			UnSlow();
		}

		// Unsprint if no longer allowed to be sprinting
		if (IsSprinting() && !CanSprintInCurrentState())
		{
			UnSprint();
		}
	}
}

void UShooterCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	// Proxies get replicated crouch and prone state.
	if (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		// Check for a change in crouch state. Players toggle crouch by changing bWantsToCrouch.
		const bool bIsCrouching = IsCrouching();
		if (bIsCrouching && (!bWantsToCrouch || !CanCrouchInCurrentState()))
		{
			UnCrouch(false);
		}
		else if (!bIsCrouching && bWantsToCrouch && CanCrouchInCurrentState())
		{
			if (IsProning())
			{
				bWantsToProne = false;
				UnProne(false);
			}
			if (IsSprinting())
			{
				bWantsToSprint = false;
				UnSprint();
			}
			Crouch(false);
		}

		// Check for a change in prone state. Players toggle prone by changing bWantsToProne.
		const bool bIsProning = IsProning();
		if (bIsProning && (!bWantsToProne || !CanProneInCurrentState()))
		{
			UnProne(false);
		}
		else if (!bIsProning && bWantsToProne && CanProneInCurrentState())
		{
			if (IsCrouching())
			{
				bWantsToCrouch = false;
				UnCrouch(false);
			}
			if (IsSlowing())
			{
				bWantsToSlow = false;
				UnSlow();
			}
			if (IsSprinting())
			{
				bWantsToSprint = false;
				UnSprint();
			}
			Prone(false);
		}

		// Check for a change in slow state. Players toggle prone by changing bWantsToSlow.
		const bool bIsSlowing = IsSlowing();
		if (bIsSlowing && (!bWantsToSlow || !CanSlowInCurrentState()))
		{
			UnSlow();
		}
		else if (!bIsSlowing && bWantsToSlow && CanSlowInCurrentState())
		{
			if (IsSprinting())
			{
				bWantsToSprint = false;
				UnSprint();
			}
			Slow();
		}

		// Check for a change in sprint state. Players toggle prone by changing bWantsToSprint.
		const bool bIsSprinting = IsSprinting();
		if (bIsSprinting && (!bWantsToSprint || !CanSprintInCurrentState()))
		{
			UnSprint();
		}
		else if (!bIsSprinting && bWantsToSprint && CanSprintInCurrentState())
		{
			if (IsCrouching())
			{
				bWantsToCrouch = false;
				UnCrouch(false);
			}
			if (IsProning())
			{
				bWantsToProne = false;
				UnProne(false);
			}
			Sprint();
		}
	}
}

bool UShooterCharacterMovementComponent::ClientUpdatePositionAfterServerUpdate()
{
	const bool bRealProne = bWantsToProne;
	const bool bRealSlow = bWantsToSlow;
	const bool bRealSprint = bWantsToSprint;
	const bool bResult = Super::ClientUpdatePositionAfterServerUpdate();

	bWantsToProne = bRealProne;
	bWantsToSlow = bRealSlow;
	bWantsToSprint = bRealSprint;

	return bResult;
}

void UShooterCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
	
	FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();

	bWantsToProne = ((Flags & FSavedMove_Character::FLAG_Custom_0) != 0);
	bWantsToSlow = ((Flags & FSavedMove_Character::FLAG_Custom_1) != 0);
	bWantsToSprint = ((Flags & FSavedMove_Character::FLAG_Custom_2) != 0);
}
