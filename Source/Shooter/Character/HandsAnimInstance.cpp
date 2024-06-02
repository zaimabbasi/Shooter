// Fill out your copyright notice in the Description page of Project Settings.


#include "HandsAnimInstance.h"
#include "Shooter/Data/WeaponDataAsset.h"
#include "Shooter/Types/LeanDirection.h"
#include "Shooter/Weapon/Weapon.h"
#include "ShooterCharacter.h"

void UHandsAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UHandsAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
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
	USkeletalMeshComponent* HandsMesh = ShooterCharacter->GetHandsMesh();
	if (CharacterMesh && HandsMesh)
	{
		// Note: Maybe no longer required because of problems (Try sprinting character with this code)
		/*FTransform CharacterWeaponRootTransform = CharacterMesh->GetSocketTransform(TEXT("Weapon_root"), ERelativeTransformSpace::RTS_Component);
		FTransform HandsWeaponRootTransform = HandsMesh->GetSocketTransform(TEXT("Weapon_root"), ERelativeTransformSpace::RTS_Component);
		FTransform DeltaWeaponRootTransform = HandsWeaponRootTransform.GetRelativeTransformReverse(CharacterWeaponRootTransform);
		HandsMesh->SetRelativeLocation(DeltaWeaponRootTransform.GetLocation());*/

		FTransform CharacterBaseHumanRibcageTransform = CharacterMesh->GetSocketTransform(TEXT("Base-HumanRibcage"), ERelativeTransformSpace::RTS_Component);
		HandsMesh->SetRelativeLocation(CharacterBaseHumanRibcageTransform.GetLocation());
	}

	AO_Yaw = ShooterCharacter->GetAO_Yaw();
	AO_Pitch = ShooterCharacter->GetAO_Pitch();

	ELeanDirection LeanDirection = ShooterCharacter->GetLeanDirection();
	float TargetLean = 0.0f;
	if (LeanDirection == ELeanDirection::LD_Left)
	{
		TargetLean = -ShooterCharacter->MaxLean;
	}
	else if (LeanDirection == ELeanDirection::LD_Right)
	{
		TargetLean = ShooterCharacter->MaxLean;
	}
	float DeltaLean = TargetLean - Lean;
	float LeanStep = ShooterCharacter->GetLeaningRate() * DeltaSeconds;
	if (FMath::Abs(DeltaLean) < LeanStep)
	{
		Lean = TargetLean;
	}
	else
	{
		if (DeltaLean < 0.0f)
		{
			LeanStep = -LeanStep;
		}
		Lean += LeanStep;
	}

	InterpAimCameraSocketLocation = ShooterCharacter->GetInterpAimCameraSocketLocation();
	
}

bool UHandsAnimInstance::HandleNotify(const FAnimNotifyEvent& AnimNotifyEvent)
{
	Super::HandleNotify(AnimNotifyEvent);

	if (ShooterCharacter == nullptr)
	{
		return false;
	}
	return ShooterCharacter->HandleHandsAnimNotify(AnimNotifyEvent);
}
