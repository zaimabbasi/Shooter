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

}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
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
	if (USkeletalMeshComponent* HandsMesh1P = OwningCharacter->GetHandsMesh1P())
	{
		if (const USkeletalMeshSocket* WeaponRootSocket = HandsMesh1P->GetSocketByName(TEXT("Weapon_rootSocket")))
		{
			WeaponRootSocket->AttachActor(WeaponToEquip, HandsMesh1P);
			WeaponToEquip->SetActorHiddenInGameWithChildren(false);
		}
	}
	EquippedWeapon = WeaponToEquip;
}
