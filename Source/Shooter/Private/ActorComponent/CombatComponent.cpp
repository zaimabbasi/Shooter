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

void UCombatComponent::SetEquippedWeapon(AWeapon* WeaponToEquip)
{
	if (!HasAuthority())
	{
		if (EquippedWeapon)
		{
			ClearWeaponDelegateBindings(EquippedWeapon);
		}
		if (WeaponToEquip)
		{
			SetWeaponDelegateBindings(WeaponToEquip);
		}
		EquippedWeapon = WeaponToEquip;
	}
	Server_SetEquippedWeapon(WeaponToEquip);
}

void UCombatComponent::Server_SetEquippedWeapon_Implementation(AWeapon* WeaponToEquip)
{
	if (EquippedWeapon)
	{
		ClearWeaponDelegateBindings(EquippedWeapon);
	}
	if (WeaponToEquip)
	{
		SetWeaponDelegateBindings(WeaponToEquip);
	}
	EquippedWeapon = WeaponToEquip;
}

void UCombatComponent::WeaponAttach(USkeletalMeshComponent* ParentSkeletalMesh, FName ParentSocketName)
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	if (!HasAuthority())
	{
		EquippedWeapon->AttachToComponent(ParentSkeletalMesh, FAttachmentTransformRules::KeepRelativeTransform, ParentSocketName);
	}
	Server_WeaponAttach(ParentSkeletalMesh, ParentSocketName);
}

void UCombatComponent::Server_WeaponAttach_Implementation(USkeletalMeshComponent* ParentSkeletalMesh, FName ParentSocketName)
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	EquippedWeapon->AttachToComponent(ParentSkeletalMesh, FAttachmentTransformRules::KeepRelativeTransform, ParentSocketName);
}

void UCombatComponent::SetWeaponDelegateBindings(AWeapon* Weapon)
{
	if (Weapon == nullptr)
	{
		return;
	}
	Weapon->OnWeaponIdle.AddDynamic(this, &UCombatComponent::Handle_OnWeaponIdle);
	Weapon->OnWeaponIdleToOut.AddDynamic(this, &UCombatComponent::Handle_OnWeaponIdleToOut);
	Weapon->OnWeaponOut.AddDynamic(this, &UCombatComponent::Handle_OnWeaponOut);
	Weapon->OnWeaponOutToIdle.AddDynamic(this, &UCombatComponent::Handle_OnWeaponOutToIdle);
	
}

void UCombatComponent::ClearWeaponDelegateBindings(AWeapon* Weapon)
{
	if (Weapon == nullptr)
	{
		return;
	}
	Weapon->OnWeaponIdle.RemoveAll(this);
	Weapon->OnWeaponIdleToOut.RemoveAll(this);
	Weapon->OnWeaponOut.RemoveAll(this);
	Weapon->OnWeaponOutToIdle.RemoveAll(this);
	
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
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));
}

void UCombatComponent::SetCombatAction(ECombatAction Action)
{
	if (!HasAuthority())
	{
		CombatAction = Action;
		if (EquippedWeapon)
		{
			EquippedWeapon->SetCombatAction(Action);
		}
	}
	Server_SetCombatAction(Action);
}

void UCombatComponent::Server_SetCombatAction_Implementation(ECombatAction Action)
{
	CombatAction = Action;
	if (EquippedWeapon)
	{
		EquippedWeapon->SetCombatAction(Action);
	}
}

void UCombatComponent::Handle_OnWeaponIdle(AWeapon* Weapon)
{
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));
}

void UCombatComponent::Handle_OnWeaponIdleToOut(AWeapon* Weapon)
{
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));
	//SetCombatAction(ECombatAction::CA_Out);
	OnCombatComponentWeaponIdleToOut.Broadcast(Weapon);
}

void UCombatComponent::Handle_OnWeaponOut(AWeapon* Weapon)
{
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));

	OnCombatComponentWeaponOut.Broadcast(Weapon);
}

void UCombatComponent::Handle_OnWeaponOutToIdle(AWeapon* Weapon)
{
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));
	SetCombatAction(ECombatAction::CA_Idle);
}

void UCombatComponent::OnRep_EquippedWeapon(AWeapon* PrevEquippedWeapon)
{

}

void UCombatComponent::Server_SetIsAiming_Implementation(bool bAiming)
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	bIsAiming = bAiming;
}
