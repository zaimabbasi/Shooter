// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterCombatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Mod/Mag.h"
#include "Weapon/Weapon.h"
#include "Character/ShooterCharacter.h"

UCharacterCombatComponent::UCharacterCombatComponent() :
	//bIsFiring(false),
	CombatAction(ECombatAction::CA_Idle),
	EquippedWeapon(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;
	
}

void UCharacterCombatComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UCharacterCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCharacterCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(UCharacterCombatComponent, bIsFiring);
	DOREPLIFETIME(UCharacterCombatComponent, CombatAction);
	DOREPLIFETIME(UCharacterCombatComponent, EquippedWeapon);

}

void UCharacterCombatComponent::ActionEnd()
{
	if (CombatAction == ECombatAction::CA_MagIn)
	{
		CombatAction = ECombatAction::CA_ActionEnd;
	}
}

void UCharacterCombatComponent::ActionStart()
{
}

void UCharacterCombatComponent::Idle()
{
	CombatAction = ECombatAction::CA_Idle;
}

void UCharacterCombatComponent::IdleToOut()
{
	if (CombatAction == ECombatAction::CA_Idle)
	{
		CombatAction = ECombatAction::CA_IdleToOut;
	}
}

void UCharacterCombatComponent::Out()
{
	if (CombatAction == ECombatAction::CA_IdleToOut)
	{
		CombatAction = ECombatAction::CA_Out;
	}
}

void UCharacterCombatComponent::OutToIdle()
{
	if (CombatAction == ECombatAction::CA_Out)
	{
		const AMag* WeaponMag = EquippedWeapon ? EquippedWeapon->GetMag() : nullptr;
		const uint8 MagAmmoCount = WeaponMag ? WeaponMag->GetAmmoCount() : 0;
		const bool bHasPatronInWeaponAmmo = EquippedWeapon && EquippedWeapon->GetPatronInWeaponAmmo() != nullptr;
		if (EquippedWeapon && MagAmmoCount > 0 && !bHasPatronInWeaponAmmo)
		{
			CombatAction = ECombatAction::CA_OutToIdleArm;
		}
		else
		{
			CombatAction = ECombatAction::CA_OutToIdle;
		}
	}
}

void UCharacterCombatComponent::WeaponChamberCheck()
{
	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon)
	{
		CombatAction = ECombatAction::CA_ChamberCheck;
	}
}

void UCharacterCombatComponent::WeaponFire(bool bFire)
{
	if (EquippedWeapon)
	{
		bWantsToFire = bFire;
		if (CombatAction == ECombatAction::CA_Idle && bWantsToFire)
		{
			CombatAction = ECombatAction::CA_Fire;
		}
	}
}

void UCharacterCombatComponent::WeaponFiremode()
{
	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon && EquippedWeapon->GetNumFiremodes() > 1)
	{
		CombatAction = ECombatAction::CA_Firemode;
	}
}

void UCharacterCombatComponent::WeaponFiremodeCheck()
{
	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon && EquippedWeapon->GetNumFiremodes() > 1)
	{
		CombatAction = ECombatAction::CA_FiremodeCheck;
	}
}

void UCharacterCombatComponent::WeaponMagCheck()
{
	const bool bHasWeaponMag = EquippedWeapon && EquippedWeapon->GetMag() != nullptr;
	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon && bHasWeaponMag)
	{
		CombatAction = ECombatAction::CA_MagCheck;
	}
}

void UCharacterCombatComponent::WeaponMagIn()
{
	const bool bHasWeaponMag = EquippedWeapon && EquippedWeapon->GetMag() != nullptr;
	if (CombatAction == ECombatAction::CA_MagOut && EquippedWeapon && bHasWeaponMag)
	{
		CombatAction = ECombatAction::CA_MagIn;
	}
}

void UCharacterCombatComponent::WeaponMagOut()
{
	const bool bHasWeaponMag = EquippedWeapon && EquippedWeapon->GetMag() != nullptr;
	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon && bHasWeaponMag)
	{
		CombatAction = ECombatAction::CA_MagOut;
	}
}

void UCharacterCombatComponent::WeaponReloadCharge()
{
	const AMag* WeaponMag = EquippedWeapon ? EquippedWeapon->GetMag() : nullptr;
	const uint8 MagAmmoCount = WeaponMag ? WeaponMag->GetAmmoCount() : 0;
	const bool bHasPatronInWeaponAmmo = EquippedWeapon && EquippedWeapon->GetPatronInWeaponAmmo() != nullptr;
	if (CombatAction == ECombatAction::CA_MagIn && EquippedWeapon && MagAmmoCount > 0 && !bHasPatronInWeaponAmmo)
	{
		CombatAction = ECombatAction::CA_ReloadCharge;
	}
}

void UCharacterCombatComponent::EquipWeapon(AWeapon* WeaponToEquip, USkeletalMeshComponent* ParentSkeletalMesh, FName InParentSocketName)
{
	if (WeaponToEquip == nullptr || EquippedWeapon)
	{
		return;
	}
	WeaponToEquip->AttachToComponent(ParentSkeletalMesh, FAttachmentTransformRules::KeepRelativeTransform, InParentSocketName);
	WeaponToEquip->bIsHolster = false;

	WeaponToEquip->OnWeaponActionEnd.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponActionEnd);
	WeaponToEquip->OnWeaponActionStart.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponActionStart);
	WeaponToEquip->OnWeaponChamberCheck.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponChamberCheck);
	WeaponToEquip->OnWeaponFire.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponFire);
	WeaponToEquip->OnWeaponFireDry.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponFireDry);
	WeaponToEquip->OnWeaponFiremode.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponFiremode);
	WeaponToEquip->OnWeaponFiremodeCheck.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponFiremodeCheck);
	WeaponToEquip->OnWeaponIdle.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponIdle);
	WeaponToEquip->OnWeaponIdleToOut.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponIdleToOut);
	WeaponToEquip->OnWeaponMagCheck.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponMagCheck);
	WeaponToEquip->OnWeaponMagIn.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponMagIn);
	WeaponToEquip->OnWeaponMagOut.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponMagOut);
	WeaponToEquip->OnWeaponOut.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponOut);
	WeaponToEquip->OnWeaponOutToIdle.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponOutToIdle);
	WeaponToEquip->OnWeaponOutToIdleArm.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponOutToIdleArm);
	WeaponToEquip->OnWeaponReloadCharge.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponReloadCharge);

	EquippedWeapon = WeaponToEquip;
}

void UCharacterCombatComponent::UnequipWeapon(USkeletalMeshComponent* ParentSkeletalMesh, FName InParentSocketName)
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	EquippedWeapon->AttachToComponent(ParentSkeletalMesh, FAttachmentTransformRules::KeepRelativeTransform, InParentSocketName);
	EquippedWeapon->bIsHolster = true;

	EquippedWeapon->OnWeaponActionEnd.RemoveAll(this);
	EquippedWeapon->OnWeaponActionStart.RemoveAll(this);
	EquippedWeapon->OnWeaponChamberCheck.RemoveAll(this);
	EquippedWeapon->OnWeaponFire.RemoveAll(this);
	EquippedWeapon->OnWeaponFireDry.RemoveAll(this);
	EquippedWeapon->OnWeaponFiremode.RemoveAll(this);
	EquippedWeapon->OnWeaponFiremodeCheck.RemoveAll(this);
	EquippedWeapon->OnWeaponIdle.RemoveAll(this);
	EquippedWeapon->OnWeaponIdleToOut.RemoveAll(this);
	EquippedWeapon->OnWeaponMagCheck.RemoveAll(this);
	EquippedWeapon->OnWeaponMagIn.RemoveAll(this);
	EquippedWeapon->OnWeaponMagOut.RemoveAll(this);
	EquippedWeapon->OnWeaponOut.RemoveAll(this);
	EquippedWeapon->OnWeaponOutToIdle.RemoveAll(this);
	EquippedWeapon->OnWeaponOutToIdleArm.RemoveAll(this);
	EquippedWeapon->OnWeaponReloadCharge.RemoveAll(this);

	EquippedWeapon = nullptr;
}

void UCharacterCombatComponent::Handle_OnWeaponActionEnd(AWeapon* Weapon)
{
	if (HasAuthority() && Weapon && Weapon == EquippedWeapon)
	{
		CombatAction = ECombatAction::CA_Idle;
	}
	OnCombatComponentWeaponActionEnd.Broadcast(Weapon);
}

void UCharacterCombatComponent::Handle_OnWeaponActionStart(AWeapon* Weapon)
{
	OnCombatComponentWeaponActionStart.Broadcast(Weapon);
}

void UCharacterCombatComponent::Handle_OnWeaponChamberCheck(AWeapon* Weapon)
{
	if (HasAuthority() && Weapon && Weapon == EquippedWeapon)
	{
		CombatAction = ECombatAction::CA_Idle;
	}
	OnCombatComponentWeaponChamberCheck.Broadcast(Weapon);
}

void UCharacterCombatComponent::Handle_OnWeaponFire(AWeapon* Weapon)
{
	if (EquippedWeapon && EquippedWeapon == Weapon)
	{
		const EWeaponFiremode WeaponFiremode = EquippedWeapon->GetFiremode();
		const bool bHasPatronInWeaponAmmo = EquippedWeapon->GetPatronInWeaponAmmo() != nullptr;
		if (!bWantsToFire || WeaponFiremode == EWeaponFiremode::WF_SingleShot/* || !bHasPatronInWeaponAmmo*/)
		{
			CombatAction = ECombatAction::CA_Idle;
		}
	}
	OnCombatComponentWeaponFire.Broadcast(Weapon);
}

void UCharacterCombatComponent::Handle_OnWeaponFireDry(AWeapon* Weapon)
{
	//if (Weapon && Weapon == EquippedWeapon)
	//{
	//	//bIsFiring = false;
	//	CombatAction = ECombatAction::CA_Idle;
	//}
	//OnCombatComponentWeaponFireDry.Broadcast(Weapon);
	if (EquippedWeapon && EquippedWeapon == Weapon)
	{
		CombatAction = ECombatAction::CA_Idle;
	}
	OnCombatComponentWeaponFireDry.Broadcast(Weapon);
}

void UCharacterCombatComponent::Handle_OnWeaponFiremode(AWeapon* Weapon)
{
	OnCombatComponentWeaponFiremode.Broadcast(Weapon);
}

void UCharacterCombatComponent::Handle_OnWeaponFiremodeCheck(AWeapon* Weapon)
{
	OnCombatComponentWeaponFiremodeCheck.Broadcast(Weapon);
}

void UCharacterCombatComponent::Handle_OnWeaponIdle(AWeapon* Weapon)
{
	OnCombatComponentWeaponIdle.Broadcast(Weapon);
}

void UCharacterCombatComponent::Handle_OnWeaponIdleToOut(AWeapon* Weapon)
{
	OnCombatComponentWeaponIdleToOut.Broadcast(Weapon);
}

void UCharacterCombatComponent::Handle_OnWeaponMagCheck(AWeapon* Weapon)
{
	if (HasAuthority() && Weapon && Weapon == EquippedWeapon)
	{
		CombatAction = ECombatAction::CA_Idle;
	}
	OnCombatComponentWeaponMagCheck.Broadcast(Weapon);
}

void UCharacterCombatComponent::Handle_OnWeaponMagIn(AWeapon* Weapon)
{
	OnCombatComponentWeaponMagIn.Broadcast(Weapon);
}

void UCharacterCombatComponent::Handle_OnWeaponMagOut(AWeapon* Weapon)
{
	OnCombatComponentWeaponMagOut.Broadcast(Weapon);
}

void UCharacterCombatComponent::Handle_OnWeaponOut(AWeapon* Weapon)
{
	OnCombatComponentWeaponOut.Broadcast(Weapon);
}

void UCharacterCombatComponent::Handle_OnWeaponOutToIdle(AWeapon* Weapon)
{
	OnCombatComponentWeaponOutToIdle.Broadcast(Weapon);
}

void UCharacterCombatComponent::Handle_OnWeaponOutToIdleArm(AWeapon* Weapon)
{
	OnCombatComponentWeaponOutToIdleArm.Broadcast(Weapon);
}

void UCharacterCombatComponent::Handle_OnWeaponReloadCharge(AWeapon* Weapon)
{
	if (HasAuthority() && Weapon && Weapon == EquippedWeapon)
	{
		CombatAction = ECombatAction::CA_Idle;
	}
	OnCombatComponentWeaponReloadCharge.Broadcast(Weapon);
}

void UCharacterCombatComponent::OnRep_CombatAction()
{
	
}

void UCharacterCombatComponent::OnRep_EquippedWeapon(AWeapon* PrevEquippedWeapon)
{
	
}
