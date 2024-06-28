// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "Shooter/Character/ShooterCharacter.h"
#include "Shooter/Weapon/Weapon.h"

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

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bIsAiming);

}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
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
	if (EquippedWeapon && OwningCharacter)
	{
		if (USkeletalMeshComponent* HandsMesh = OwningCharacter->GetHandsMesh())
		{
			HandsMesh->SetAnimClass(EquippedWeapon->GetHandsAnimClass());
		}
	}
}

void UCombatComponent::Server_SetEquippedWeapon_Implementation(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr || OwningCharacter == nullptr)
	{
		return;
	}
	if (EquippedWeapon)
	{
		EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		EquippedWeapon->SetActorHiddenInGameWithChildren(true);
	}
	if (USkeletalMeshComponent* HandsMesh = OwningCharacter->GetHandsMesh())
	{
		if (const USkeletalMeshSocket* WeaponRootSocket = HandsMesh->GetSocketByName(TEXT("Weapon_rootSocket")))
		{
			WeaponRootSocket->AttachActor(WeaponToEquip, HandsMesh);
			WeaponToEquip->SetActorHiddenInGameWithChildren(false);
		}
		HandsMesh->SetAnimClass(WeaponToEquip->GetHandsAnimClass());
	}
	EquippedWeapon = WeaponToEquip;
}

void UCombatComponent::Server_SetIsAiming_Implementation(bool bAiming)
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	bIsAiming = bAiming;
}
