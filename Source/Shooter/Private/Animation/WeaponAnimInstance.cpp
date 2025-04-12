// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/WeaponAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/ShooterCharacter.h"
#include "Components/CharacterCombatComponent.h"
#include "Types/ShooterNames.h"
#include "Utility/ShooterUtility.h"
#include "Weapon/Weapon.h"

void UWeaponAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Weapon = Cast<AWeapon>(GetOwningActor());

	if (Weapon)
	{
		FireAnimPlayRate = Weapon->GetRateOfFire() / 60.0f;
		//FireAnimPlayRate = 1.0f;
	}

	CombatAction = ECombatAction::CA_None;

	IKAlpha = 0.0f;
	IKBlendInOutFlag = 0;
	IKBlendDuration = 0.3f;
	IKBlendDurationCounter = 0.0f;

	IdleAnimMaxHorizontalMovement = 0.2f;
	IdleAnimMinHorizontalMovement = -0.2f;
	IdleAnimMaxVerticalMovement = 0.25f;
	IdleAnimMinVerticalMovement = -0.5f;
	IdleAnimMaxRollRotation = 1.5f;
	IdleAnimMinRollRotation = -1.5f;

	WalkAnimMaxHorizontalMovement = 0.3f;
	WalkAnimMinHorizontalMovement = -0.3f;
	WalkAnimMaxVerticalMovement = 0.25f;
	WalkAnimMinVerticalMovement = -0.5f;
	WalkAnimMaxRollRotation = 2.5f;
	WalkAnimMinRollRotation = -2.5f;

	SwayHorizontalMovementLimit = 1.0f;
	SwayVerticalMovementLimit = 1.0f;
	SwayRollRotationLimit = 7.5f;

	SwayHorizontalMovementSensitivity = 0.025;
	SwayVerticalMovementSensitivity = 0.025;
	SwayRollRotationSensitivity = 0.075;

	SwayInterpSpeed = 5.0f;

}

void UWeaponAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Weapon == nullptr)
	{
		Weapon = Cast<AWeapon>(GetOwningActor());
	}
	if (Weapon == nullptr)
	{
		return;
	}

	CharacterViewRotationLast = CharacterViewRotation;

	SwayHorizontalMovement = FMath::FInterpTo(SwayHorizontalMovement, 0.0f, DeltaSeconds, SwayInterpSpeed);
	SwayVerticalMovement = FMath::FInterpTo(SwayVerticalMovement, 0.0f, DeltaSeconds, SwayInterpSpeed);
	SwayRollRotation = FMath::FInterpTo(SwayRollRotation, 0.0f, DeltaSeconds, SwayInterpSpeed);

	ForegripHandguardMesh = Weapon->GetForegripHandguardMesh();
	bHasForegripHandguardMesh = ForegripHandguardMesh != nullptr;
	Firemode = Weapon->GetFiremode();
	bIsHolster = Weapon->GetIsHolster();
	bHasPatronInWeaponAmmo = Weapon->GetPatronInWeaponAmmo() != nullptr;
	bIsPistol = Weapon->IsPistol();
	bIsOneHanded = Weapon->GetIsOneHanded();
	IdleAnimHorizontalMovementAlpha = Weapon->GetIdleAnimHorizontalMovementAlpha();
	IdleAnimVerticalMovementAlpha = Weapon->GetIdleAnimVerticalMovementAlpha();
	IdleAnimRollRotationAlpha = Weapon->GetIdleAnimRollRotationAlpha();
	WalkAnimHorizontalMovementAlpha = Weapon->GetWalkAnimHorizontalMovementAlpha();
	WalkAnimVerticalMovementAlpha = Weapon->GetWalkAnimVerticalMovementAlpha();
	WalkAnimRollRotationAlpha = Weapon->GetWalkAnimRollRotationAlpha();
	bIsPlayingProceduralIdle = Weapon->IsPlayingProceduralIdle();
	bIsPlayingProceduralWalk = Weapon->IsPlayingProceduralWalk();

	AShooterCharacter* ShooterCharacter = Weapon->GetShooterCharacterOwner();
	CharacterMesh = ShooterCharacter ? ShooterCharacter->GetMesh() : nullptr;
	bIsCharacterProned = ShooterCharacter && ShooterCharacter->bIsProned;
	bIsCharacterSprinting = ShooterCharacter && ShooterCharacter->bIsSprinting;

	FVector CharacterVelocity = ShooterCharacter != nullptr ? ShooterCharacter->GetVelocity() : FVector::ZeroVector;
	bHasCharacterVelocity = CharacterVelocity.SizeSquared2D() > 0.0f;

	bool bIsCharacterTransition = ShooterCharacter && ShooterCharacter->GetIsTransition();
	bIsCharacterThirdAction = bIsCharacterSprinting || bIsCharacterTransition || (bIsCharacterProned && bHasCharacterVelocity);

	CharacterViewRotation = ShooterCharacter != nullptr ? ShooterCharacter->GetBaseAimRotation() : FRotator::ZeroRotator;

	FRotator ViewRotationDelta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterViewRotation, CharacterViewRotationLast);
	SwayHorizontalMovement += ViewRotationDelta.Yaw * SwayHorizontalMovementSensitivity;
	SwayHorizontalMovement = FMath::Clamp(SwayHorizontalMovement, -SwayHorizontalMovementLimit, SwayHorizontalMovementLimit);

	SwayVerticalMovement += ViewRotationDelta.Pitch * SwayVerticalMovementSensitivity;
	SwayVerticalMovement = FMath::Clamp(SwayVerticalMovement, -SwayVerticalMovementLimit, SwayVerticalMovementLimit);

	SwayRollRotation += ViewRotationDelta.Yaw * SwayRollRotationSensitivity;
	SwayRollRotation = FMath::Clamp(SwayRollRotation, -SwayRollRotationLimit, SwayRollRotationLimit);

	UCharacterCombatComponent* CharacterCombatComponent = ShooterCharacter ? ShooterCharacter->GetCharacterCombatComponent() : nullptr;
	AWeapon* CharacterEquippedWeapon = CharacterCombatComponent ? CharacterCombatComponent->GetEquippedWeapon() : nullptr;
	CombatAction = CharacterCombatComponent && Weapon == CharacterEquippedWeapon ? CharacterCombatComponent->GetCombatAction() : ECombatAction::CA_Out;

	if (bIsCharacterThirdAction)
	{
		IKAlpha = 1.0f;
	}
	else
	{
		if (bHasForegripHandguardMesh)
		{
			IKBlendDurationCounter += IKBlendInOutFlag * DeltaSeconds;
			IKBlendDurationCounter = FMath::Clamp(IKBlendDurationCounter, 0.0f, IKBlendDuration);
			IKAlpha = IKBlendDurationCounter / IKBlendDuration;
		}
		else
		{
			IKAlpha = 0.0f;
		}
	}

	if (ForegripHandguardMesh)
	{
		LPalmTransform = ForegripHandguardMesh->GetSocketTransform(BASE_HUMAN_L_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
	}
	if (CharacterMesh && bIsCharacterThirdAction)
	{
		BendGoalLeftTransform = CharacterMesh->GetSocketTransform(BEND_GOAL_LEFT_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		BendGoalRightTransform = CharacterMesh->GetSocketTransform(BEND_GOAL_RIGHT_SOCKET_NAME, ERelativeTransformSpace::RTS_World);

		LCollarboneTransform = CharacterMesh->GetSocketTransform(L_COLLARBONE_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		RCollarboneTransform = CharacterMesh->GetSocketTransform(R_COLLARBONE_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);

		if ((bIsCharacterProned && bHasCharacterVelocity) || (bIsCharacterSprinting && (bIsPistol || bIsOneHanded)))
		{
			LPalmTransform = CharacterMesh->GetSocketTransform(IK_S_L_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
			RPalmTransform = CharacterMesh->GetSocketTransform(IK_S_R_PALM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		}

		WeaponRootAnimTransform = CharacterMesh->GetSocketTransform(WEAPON_ROOT_3RD_ANIM_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
	}

	/*FRotator CharacterRotation = ShooterCharacter != nullptr ? ShooterCharacter->GetActorRotation() : FRotator::ZeroRotator;
	if (bHasCharacterVelocity)
	{
		float VelocityYaw = UKismetMathLibrary::MakeRotFromX(CharacterVelocity).Yaw;
		float VelocityYawOffset = UKismetMathLibrary::NormalizedDeltaRotator(FRotator(0.0f, VelocityYaw, 0.0f), FRotator(0.0f, CharacterRotation.Yaw, 0.0f)).Yaw;

		float CharacterVelocityYawOffsetAlpha = UKismetMathLibrary::NormalizeToRange(VelocityYawOffset, 0.0f, 90.0f);
		if (FMath::Abs(CharacterVelocityYawOffsetAlpha) > 1.0f)
		{
			FVector2D InRange = FVector2D(1.0f, 2.0f);
			FVector2D OutRange = FVector2D(1.0, 0.0);
			if (CharacterVelocityYawOffsetAlpha < 0.0f)
			{
				InRange *= -1;
				OutRange *= -1;
			}
			CharacterVelocityYawOffsetAlpha = FMath::GetMappedRangeValueClamped(InRange, OutRange, CharacterVelocityYawOffsetAlpha);
		}

		float WalkAnimMaxHorizontalMovementOffset;
		float WalkAnimMinHorizontalMovementOffset;
		if (CharacterVelocityYawOffsetAlpha > 0.0f)
		{
			WalkAnimMaxHorizontalMovementOffset = FMath::FInterpTo(WalkAnimMaxHorizontalMovementOffset, WalkAnimMaxHorizontalMovement * CharacterVelocityYawOffsetAlpha, DeltaSeconds, 5.0f);
			WalkAnimMinHorizontalMovementOffset = FMath::FInterpTo(WalkAnimMinHorizontalMovementOffset, 0, DeltaSeconds, 5.0f);
		}
		else if (CharacterVelocityYawOffsetAlpha < 0.0f)
		{
			WalkAnimMaxHorizontalMovementOffset = FMath::FInterpTo(WalkAnimMaxHorizontalMovementOffset, 0, DeltaSeconds, 5.0f);
			WalkAnimMinHorizontalMovementOffset = FMath::FInterpTo(WalkAnimMinHorizontalMovementOffset, WalkAnimMinHorizontalMovement * CharacterVelocityYawOffsetAlpha, DeltaSeconds, 5.0f);
		}
	}*/
	
}

void UWeaponAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);

	if (Weapon == nullptr)
	{
		return;
	}	

	if (!bIsHolster)
	{
		if (CombatAction == ECombatAction::CA_Idle && !bIsCharacterThirdAction)
		{
			if (!bIsPlayingProceduralIdle && !bHasCharacterVelocity)
			{
				OnWeaponAnimInstancePlayProceduralIdle.Broadcast();
			}
			else if (!bIsPlayingProceduralWalk && bHasCharacterVelocity)
			{
				OnWeaponAnimInstancePlayProceduralWalk.Broadcast();
			}
			else if (bIsPlayingProceduralIdle || bIsPlayingProceduralWalk)
			{
				FVector ProceduralAnimLocation;
				FRotator ProceduralAnimRotation;
				if (bIsPlayingProceduralIdle)
				{
					ProceduralAnimLocation.X = UKismetMathLibrary::Lerp(IdleAnimMinHorizontalMovement, IdleAnimMaxHorizontalMovement, IdleAnimHorizontalMovementAlpha);
					ProceduralAnimLocation.Z = UKismetMathLibrary::Lerp(IdleAnimMinVerticalMovement, IdleAnimMaxVerticalMovement, IdleAnimVerticalMovementAlpha);
					ProceduralAnimRotation.Pitch = UKismetMathLibrary::Lerp(IdleAnimMinRollRotation, IdleAnimMaxRollRotation, IdleAnimRollRotationAlpha);
					
				}
				else if (bIsPlayingProceduralWalk)
				{
					ProceduralAnimLocation.X = UKismetMathLibrary::Lerp(WalkAnimMinHorizontalMovement, WalkAnimMaxHorizontalMovement, WalkAnimHorizontalMovementAlpha);
					ProceduralAnimLocation.Z = UKismetMathLibrary::Lerp(WalkAnimMinVerticalMovement, WalkAnimMaxVerticalMovement, WalkAnimVerticalMovementAlpha);
					ProceduralAnimRotation.Pitch = UKismetMathLibrary::Lerp(WalkAnimMinRollRotation, WalkAnimMaxRollRotation, WalkAnimRollRotationAlpha);
				}

				ProceduralAnimLocation.X += SwayHorizontalMovement;
				ProceduralAnimLocation.Z += -SwayVerticalMovement;
				ProceduralAnimRotation.Pitch += SwayRollRotation;
				
				WeaponRootAnimTransform.SetLocation(ProceduralAnimLocation);
				WeaponRootAnimTransform.SetRotation(FQuat(ProceduralAnimRotation));
			}
		}
		else if (bIsPlayingProceduralIdle || bIsPlayingProceduralWalk)
		{
			OnWeaponAnimInstanceStopProceduralAnim.Broadcast();
		}
		
	}

}

void UWeaponAnimInstance::AnimNotify_ActionEnd() const
{
	OnWeaponAnimInstanceActionEnd.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_ActionStart() const
{
	OnWeaponAnimInstanceActionStart.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_ChamberCheck() const
{
	OnWeaponAnimInstanceChamberCheck.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_Fire() const
{
	OnWeaponAnimInstanceFire.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_FireDry() const
{
	OnWeaponAnimInstanceFireDry.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_Firemode() const
{
	OnWeaponAnimInstanceFiremode.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_FiremodeCheck() const
{
	OnWeaponAnimInstanceFiremodeCheck.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_Idle() const
{
	OnWeaponAnimInstanceIdle.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_IdleToOut() const
{
	OnWeaponAnimInstanceIdleToOut.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_MagCheck() const
{
	OnWeaponAnimInstanceMagCheck.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_MagIn() const
{
	OnWeaponAnimInstanceMagIn.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_MagOut() const
{
	OnWeaponAnimInstanceMagOut.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_Out() const
{
	OnWeaponAnimInstanceOut.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_OutToIdle() const
{
	OnWeaponAnimInstanceOutToIdle.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_OutToIdleArm() const
{
	OnWeaponAnimInstanceOutToIdleArm.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_ReloadCharge() const
{
	OnWeaponAnimInstanceReloadCharge.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_PatronInWeapon() const
{
	OnWeaponAnimInstancePatronInWeapon.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_WeaponSelector() const
{
	OnWeaponAnimInstanceWeaponSelector.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_WeaponHammer() const
{
	OnWeaponAnimInstanceWeaponHammer.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_ShellPort() const
{
	OnWeaponAnimInstanceShellPort.Broadcast();
}

void UWeaponAnimInstance::AnimNotify_WeaponLHandMarker()
{
	IKBlendInOutFlag = 1;
}

void UWeaponAnimInstance::AnimNotify_WeaponLIKMarker()
{
	IKBlendInOutFlag = -1;
}
