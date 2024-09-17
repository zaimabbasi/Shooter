// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/CombatComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "Actor/Weapon.h"
#include "Character/ShooterCharacter.h"

UCombatComponent::UCombatComponent() :
	EquippedWeapon(nullptr),
	CombatAction(ECombatAction::CA_Idle)
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
	DOREPLIFETIME(UCombatComponent, CombatAction);

}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCombatComponent::Server_WeaponIdle_Implementation()
{
	if (CanIdle())
	{
		SetCombatAction(ECombatAction::CA_Idle);
	}
}

void UCombatComponent::Server_WeaponIdleToOut_Implementation()
{
	if (CanIdleToOut())
	{
		SetCombatAction(ECombatAction::CA_IdleToOut);
	}
}

void UCombatComponent::Server_WeaponOut_Implementation()
{
	if (CanOut())
	{
		SetCombatAction(ECombatAction::CA_Out);
	}
}

void UCombatComponent::Server_WeaponOutToIdle_Implementation()
{
	if (CanOutToIdle())
	{
		SetCombatAction(DoesWeaponNeedCharge() ? ECombatAction::CA_OutToIdleArm : ECombatAction::CA_OutToIdle);
	}
}

void UCombatComponent::Server_EquipWeapon_Implementation(AWeapon* WeaponToEquip, USkeletalMeshComponent* ParentSkeletalMesh, FName InParentSocketName)
{
	if (WeaponToEquip == nullptr || EquippedWeapon)
	{
		return;
	}
	WeaponToEquip->AttachToComponent(ParentSkeletalMesh, FAttachmentTransformRules::KeepRelativeTransform, InParentSocketName);
	WeaponToEquip->Server_SetIsHolster(false);

	WeaponToEquip->OnWeaponIdle.AddDynamic(this, &UCombatComponent::Handle_OnWeaponIdle);
	WeaponToEquip->OnWeaponIdleToOut.AddDynamic(this, &UCombatComponent::Handle_OnWeaponIdleToOut);
	WeaponToEquip->OnWeaponOut.AddDynamic(this, &UCombatComponent::Handle_OnWeaponOut);
	WeaponToEquip->OnWeaponOutToIdle.AddDynamic(this, &UCombatComponent::Handle_OnWeaponOutToIdle);

	EquippedWeapon = WeaponToEquip;
}

void UCombatComponent::Server_UnequipWeapon_Implementation(USkeletalMeshComponent* ParentSkeletalMesh, FName InParentSocketName)
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	EquippedWeapon->AttachToComponent(ParentSkeletalMesh, FAttachmentTransformRules::KeepRelativeTransform, InParentSocketName);
	EquippedWeapon->Server_SetIsHolster(true);

	EquippedWeapon->OnWeaponIdle.RemoveAll(this);
	EquippedWeapon->OnWeaponIdleToOut.RemoveAll(this);
	EquippedWeapon->OnWeaponOut.RemoveAll(this);
	EquippedWeapon->OnWeaponOutToIdle.RemoveAll(this);

	EquippedWeapon = nullptr;
}

void UCombatComponent::SetIsAiming(bool bAiming)
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	if (!HasAuthority())
	{
		bIsAiming = bAiming;
	}
	Server_SetIsAiming(bAiming);
}

void UCombatComponent::Server_SetIsAiming_Implementation(bool bAiming)
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	bIsAiming = bAiming;
}

void UCombatComponent::ReloadWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));
}

void UCombatComponent::SetCombatAction(ECombatAction Action)
{
	CombatAction = Action;
	if (EquippedWeapon)
	{
		EquippedWeapon->SetCombatAction(Action);
	}
}

bool UCombatComponent::CanIdle() const
{
	return (CombatAction == ECombatAction::CA_OutToIdle || CombatAction == ECombatAction::CA_OutToIdleArm);
}

bool UCombatComponent::CanIdleToOut() const
{
	return (CombatAction == ECombatAction::CA_Idle);
}

bool UCombatComponent::CanOut() const
{
	return (CombatAction == ECombatAction::CA_IdleToOut);
}

bool UCombatComponent::CanOutToIdle() const
{
	return (CombatAction == ECombatAction::CA_Out);
}

bool UCombatComponent::CanWeaponFire() const
{
	return (CombatAction == ECombatAction::CA_Idle && EquippedWeapon && EquippedWeapon->CanFire());
}

bool UCombatComponent::DoesWeaponNeedCharge() const
{
	return (EquippedWeapon && EquippedWeapon->DoesNeedCharge());
}

void UCombatComponent::Handle_OnWeaponIdle(AWeapon* Weapon)
{
	OnCombatComponentWeaponIdle.Broadcast(Weapon);
}

void UCombatComponent::Handle_OnWeaponIdleToOut(AWeapon* Weapon)
{
	OnCombatComponentWeaponIdleToOut.Broadcast(Weapon);
}

void UCombatComponent::Handle_OnWeaponOut(AWeapon* Weapon)
{
	OnCombatComponentWeaponOut.Broadcast(Weapon);
}

void UCombatComponent::Handle_OnWeaponOutToIdle(AWeapon* Weapon)
{
	OnCombatComponentWeaponOutToIdle.Broadcast(Weapon);
}

void UCombatComponent::OnRep_EquippedWeapon(AWeapon* PrevEquippedWeapon)
{
	if (EquippedWeapon)
	{
		// In case when Authoritative Character's EquippedWeapon is null for Authonomous Characters
		EquippedWeapon->SetCombatAction(CombatAction);
	}
}

void UCombatComponent::OnRep_CombatAction()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->SetCombatAction(CombatAction);
	}
	
}
