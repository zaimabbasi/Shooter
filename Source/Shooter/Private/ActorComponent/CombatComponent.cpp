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

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
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

void UCombatComponent::Server_SetEquippedWeapon_Implementation(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr)
	{
		return;
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
