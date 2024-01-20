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

	DOREPLIFETIME_CONDITION(UCombatComponent, EquippedWeapon, COND_OwnerOnly);

}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

//void UCombatComponent::SetEquippedWeapon1P(AWeapon* WeaponToEquip1P)
//{
//	if (WeaponToEquip1P == nullptr || OwningCharacter == nullptr)
//	{
//		return;
//	}
//	if (EquippedWeapon1P)
//	{
//		EquippedWeapon1P->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
//		EquippedWeapon1P->SetActorHiddenInGame(true);
//	}
//	if (USkeletalMeshComponent* HandsMesh1P = OwningCharacter->GetHandsMesh1P())
//	{
//		if (const USkeletalMeshSocket* WeaponRootSocket = HandsMesh1P->GetSocketByName(TEXT("Weapon_rootSocket")))
//		{
//			WeaponRootSocket->AttachActor(WeaponToEquip1P, HandsMesh1P);
//			WeaponToEquip1P->SetActorHiddenInGame(false);
//		}
//	}
//	EquippedWeapon1P = WeaponToEquip1P;
//}

void UCombatComponent::Server_SetEquippedWeapon_Implementation(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr || OwningCharacter == nullptr)
	{
		return;
	}
	if (EquippedWeapon)
	{
		EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
		EquippedWeapon->SetHiddenInGame(true, true);
	}
	if (USkeletalMeshComponent* HandsMesh1P = OwningCharacter->GetHandsMesh1P())
	{
		if (const USkeletalMeshSocket* WeaponRootSocket = HandsMesh1P->GetSocketByName(TEXT("Weapon_rootSocket")))
		{
			WeaponRootSocket->AttachActor(WeaponToEquip, HandsMesh1P);
			WeaponToEquip->SetHiddenInGame(false, true);
		}
	}
	/*if (USkeletalMeshComponent* CharacterMesh3P = OwningCharacter->GetMesh())
	{
		if (const USkeletalMeshSocket* WeaponRootSocket = CharacterMesh3P->GetSocketByName(TEXT("Weapon_rootSocket")))
		{
			WeaponRootSocket->AttachActor(WeaponToEquip, CharacterMesh3P);
			WeaponToEquip->SetActorHiddenInGame(false);
		}
	}*/
	EquippedWeapon = WeaponToEquip;
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_EquippedWeapon"));
	
}
