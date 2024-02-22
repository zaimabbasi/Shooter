// Fill out your copyright notice in the Description page of Project Settings.


#include "HandsAnimInstance.h"
#include "Shooter/Data/WeaponDataAsset.h"
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

	if (AWeapon* EquippedWeapon = ShooterCharacter->GetEquippedWeapon())
	{
		if (UHandsAnimationDataAsset* HandsAnimationDataAsset = EquippedWeapon->GetHandsAnimationDataAsset())
		{
			IdleAnim = HandsAnimationDataAsset->IdleAnim;
		}
	}

	USkeletalMeshComponent* CharacterMesh = ShooterCharacter->GetMesh();
	USkeletalMeshComponent* HandsMesh = ShooterCharacter->GetHandsMesh();
	if (CharacterMesh && HandsMesh)
	{
		FTransform CharacterWeaponRootTransform = CharacterMesh->GetSocketTransform(TEXT("Weapon_root"), ERelativeTransformSpace::RTS_Component);
		FTransform HandsWeaponRootTransform = HandsMesh->GetSocketTransform(TEXT("Weapon_root"), ERelativeTransformSpace::RTS_Component);
		FTransform DeltaWeaponRootTransform = HandsWeaponRootTransform.GetRelativeTransformReverse(CharacterWeaponRootTransform);
		HandsMesh->SetRelativeLocation(DeltaWeaponRootTransform.GetLocation());
	}

	AO_Yaw = ShooterCharacter->GetAO_Yaw();
	AO_Pitch = ShooterCharacter->GetAO_Pitch();

}
