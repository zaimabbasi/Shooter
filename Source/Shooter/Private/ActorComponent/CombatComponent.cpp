// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/CombatComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "Actor/Weapon.h"
#include "Character/ShooterCharacter.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UCombatComponent, EquippedWeapon, COND_OwnerOnly);
	DOREPLIFETIME(UCombatComponent, bIsAiming);

}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCombatComponent::SetEquippedWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr)
	{
		return;
	}
	EquippedWeapon = WeaponToEquip;
	Server_SetEquippedWeapon(WeaponToEquip);
}

void UCombatComponent::SetIsAiming(bool bAiming)
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	bIsAiming = bAiming;
	Server_SetIsAiming(bAiming);
}

void UCombatComponent::ReloadWeapon()
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("ReloadWeapon"));
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	OnRepEquippedWeaponDelegate.Broadcast(EquippedWeapon);
}

void UCombatComponent::Server_SetEquippedWeapon_Implementation(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr)
	{
		return;
	}
	EquippedWeapon = WeaponToEquip;
}
//void UCombatComponent::Server_SetEquippedWeapon_Implementation(AWeapon* WeaponToEquip)
//{
//	if (WeaponToEquip == nullptr || OwningCharacter == nullptr)
//	{
//		return;
//	}
//	if (EquippedWeapon)
//	{
//		EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
//		FName WeaponHolsterSocketName = TEXT("weapon_holsterSocket");
//		if (EquippedWeapon->IsPistol())
//		{
//			WeaponHolsterSocketName = TEXT("pistol_holsterSocket");
//		}
//		//else if (!OwningCharacter->IsPrimaryWeapon(EquippedWeapon))
//		{
//			WeaponHolsterSocketName = TEXT("weapon_holster1Socket");
//		}
//		if (USkeletalMeshComponent* CharacterMesh = OwningCharacter->GetMesh())
//		{
//			if (const USkeletalMeshSocket* WeaponHolsterSocket = CharacterMesh->GetSocketByName(WeaponHolsterSocketName))
//			{
//				WeaponHolsterSocket->AttachActor(EquippedWeapon, CharacterMesh);
//			}
//		}
//	}
//	if (USkeletalMeshComponent* HandsMesh = OwningCharacter->GetHandsMesh())
//	{
//		if (const USkeletalMeshSocket* WeaponRootSocket = HandsMesh->GetSocketByName(TEXT("Weapon_rootSocket")))
//		{
//			WeaponRootSocket->AttachActor(WeaponToEquip, HandsMesh);
//		}
//		HandsMesh->SetAnimClass(WeaponToEquip->GetHandsAnimClass());
//	}
//	EquippedWeapon = WeaponToEquip;
//}

void UCombatComponent::Server_SetIsAiming_Implementation(bool bAiming)
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	bIsAiming = bAiming;
}
