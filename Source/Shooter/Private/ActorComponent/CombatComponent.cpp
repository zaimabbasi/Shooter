// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/CombatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Actor/Mag.h"
#include "Actor/Weapon.h"
#include "Character/ShooterCharacter.h"
#include "Enum/WeaponFiremode.h"

UCombatComponent::UCombatComponent() :
	bIsFiring(false),
	CombatAction(ECombatAction::CA_Idle),
	EquippedWeapon(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;
	
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, bIsFiring);
	DOREPLIFETIME(UCombatComponent, CombatAction);
	DOREPLIFETIME(UCombatComponent, EquippedWeapon);

}

void UCombatComponent::ActionEnd()
{
	if (CombatAction == ECombatAction::CA_MagIn)
	{
		CombatAction = ECombatAction::CA_ActionEnd;
	}
}

void UCombatComponent::ActionStart()
{
}

void UCombatComponent::Idle()
{
	CombatAction = ECombatAction::CA_Idle;
}

void UCombatComponent::IdleToOut()
{
	if (CombatAction == ECombatAction::CA_Idle)
	{
		CombatAction = ECombatAction::CA_IdleToOut;
	}
}

void UCombatComponent::Out()
{
	if (CombatAction == ECombatAction::CA_IdleToOut)
	{
		CombatAction = ECombatAction::CA_Out;
	}
}

void UCombatComponent::OutToIdle()
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

void UCombatComponent::WeaponChamberCheck()
{
	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon)
	{
		CombatAction = ECombatAction::CA_ChamberCheck;
	}
}

void UCombatComponent::WeaponFire(bool bFiring)
{
	if (EquippedWeapon)
	{
		if (CombatAction == ECombatAction::CA_Idle && bFiring)
		{
			bIsFiring = true;
			CombatAction = ECombatAction::CA_Fire;
		}
		else if (!bFiring)
		{
			bIsFiring = false;
		}
	}
}

void UCombatComponent::WeaponFiremode()
{
	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon && EquippedWeapon->GetNumFiremodes() > 1)
	{
		CombatAction = ECombatAction::CA_Firemode;
	}
}

void UCombatComponent::WeaponFiremodeCheck()
{
	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon && EquippedWeapon->GetNumFiremodes() > 1)
	{
		CombatAction = ECombatAction::CA_FiremodeCheck;
	}
}

void UCombatComponent::WeaponMagCheck()
{
	const bool bHasWeaponMag = EquippedWeapon && EquippedWeapon->GetMag() != nullptr;
	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon && bHasWeaponMag)
	{
		CombatAction = ECombatAction::CA_MagCheck;
	}
}

void UCombatComponent::WeaponMagIn()
{
	const bool bHasWeaponMag = EquippedWeapon && EquippedWeapon->GetMag() != nullptr;
	if (CombatAction == ECombatAction::CA_MagOut && EquippedWeapon && bHasWeaponMag)
	{
		CombatAction = ECombatAction::CA_MagIn;
	}
}

void UCombatComponent::WeaponMagOut()
{
	const bool bHasWeaponMag = EquippedWeapon && EquippedWeapon->GetMag() != nullptr;
	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon && bHasWeaponMag)
	{
		CombatAction = ECombatAction::CA_MagOut;
	}
}

void UCombatComponent::WeaponReloadCharge()
{
	const AMag* WeaponMag = EquippedWeapon ? EquippedWeapon->GetMag() : nullptr;
	const uint8 MagAmmoCount = WeaponMag ? WeaponMag->GetAmmoCount() : 0;
	const bool bHasPatronInWeaponAmmo = EquippedWeapon && EquippedWeapon->GetPatronInWeaponAmmo() != nullptr;
	if (CombatAction == ECombatAction::CA_MagIn && EquippedWeapon && MagAmmoCount > 0 && !bHasPatronInWeaponAmmo)
	{
		CombatAction = ECombatAction::CA_ReloadCharge;
	}
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip, USkeletalMeshComponent* ParentSkeletalMesh, FName InParentSocketName)
{
	if (WeaponToEquip == nullptr || EquippedWeapon)
	{
		return;
	}
	WeaponToEquip->AttachToComponent(ParentSkeletalMesh, FAttachmentTransformRules::KeepRelativeTransform, InParentSocketName);
	WeaponToEquip->bIsHolster = false;

	WeaponToEquip->OnWeaponActionEnd.AddDynamic(this, &UCombatComponent::Handle_OnWeaponActionEnd);
	WeaponToEquip->OnWeaponActionStart.AddDynamic(this, &UCombatComponent::Handle_OnWeaponActionStart);
	WeaponToEquip->OnWeaponChamberCheck.AddDynamic(this, &UCombatComponent::Handle_OnWeaponChamberCheck);
	WeaponToEquip->OnWeaponFire.AddDynamic(this, &UCombatComponent::Handle_OnWeaponFire);
	WeaponToEquip->OnWeaponFireDry.AddDynamic(this, &UCombatComponent::Handle_OnWeaponFireDry);
	WeaponToEquip->OnWeaponFiremode.AddDynamic(this, &UCombatComponent::Handle_OnWeaponFiremode);
	WeaponToEquip->OnWeaponFiremodeCheck.AddDynamic(this, &UCombatComponent::Handle_OnWeaponFiremodeCheck);
	WeaponToEquip->OnWeaponIdle.AddDynamic(this, &UCombatComponent::Handle_OnWeaponIdle);
	WeaponToEquip->OnWeaponIdleToOut.AddDynamic(this, &UCombatComponent::Handle_OnWeaponIdleToOut);
	WeaponToEquip->OnWeaponMagCheck.AddDynamic(this, &UCombatComponent::Handle_OnWeaponMagCheck);
	WeaponToEquip->OnWeaponMagIn.AddDynamic(this, &UCombatComponent::Handle_OnWeaponMagIn);
	WeaponToEquip->OnWeaponMagOut.AddDynamic(this, &UCombatComponent::Handle_OnWeaponMagOut);
	WeaponToEquip->OnWeaponOut.AddDynamic(this, &UCombatComponent::Handle_OnWeaponOut);
	WeaponToEquip->OnWeaponOutToIdle.AddDynamic(this, &UCombatComponent::Handle_OnWeaponOutToIdle);
	WeaponToEquip->OnWeaponOutToIdleArm.AddDynamic(this, &UCombatComponent::Handle_OnWeaponOutToIdleArm);
	WeaponToEquip->OnWeaponReloadCharge.AddDynamic(this, &UCombatComponent::Handle_OnWeaponReloadCharge);

	EquippedWeapon = WeaponToEquip;
}

void UCombatComponent::UnequipWeapon(USkeletalMeshComponent* ParentSkeletalMesh, FName InParentSocketName)
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

void UCombatComponent::Handle_OnWeaponActionEnd(AWeapon* Weapon)
{
	if (HasAuthority() && Weapon && Weapon == EquippedWeapon)
	{
		CombatAction = ECombatAction::CA_Idle;
	}
	OnCombatComponentWeaponActionEnd.Broadcast(Weapon);
}

void UCombatComponent::Handle_OnWeaponActionStart(AWeapon* Weapon)
{
	OnCombatComponentWeaponActionStart.Broadcast(Weapon);
}

void UCombatComponent::Handle_OnWeaponChamberCheck(AWeapon* Weapon)
{
	if (HasAuthority() && Weapon && Weapon == EquippedWeapon)
	{
		CombatAction = ECombatAction::CA_Idle;;
	}
	OnCombatComponentWeaponChamberCheck.Broadcast(Weapon);
}

void UCombatComponent::Handle_OnWeaponFire(AWeapon* Weapon)
{
	if (Weapon && Weapon == EquippedWeapon)
	{
		const EWeaponFiremode WeaponFiremode = Weapon->GetFiremode();
		const bool bHasPatronInWeaponAmmo = Weapon->GetPatronInWeaponAmmo() != nullptr;
		if (WeaponFiremode == EWeaponFiremode::WF_SingleShot || !bHasPatronInWeaponAmmo)
		{
			bIsFiring = false;
			CombatAction = ECombatAction::CA_Idle;
		}
		if (!bIsFiring)
		{
			CombatAction = ECombatAction::CA_Idle;
		}
	}
	OnCombatComponentWeaponFire.Broadcast(Weapon);
}

void UCombatComponent::Handle_OnWeaponFireDry(AWeapon* Weapon)
{
	if (Weapon && Weapon == EquippedWeapon)
	{
		bIsFiring = false;
		CombatAction = ECombatAction::CA_Idle;
	}
	OnCombatComponentWeaponFireDry.Broadcast(Weapon);
}

void UCombatComponent::Handle_OnWeaponFiremode(AWeapon* Weapon)
{
	OnCombatComponentWeaponFiremode.Broadcast(Weapon);
}

void UCombatComponent::Handle_OnWeaponFiremodeCheck(AWeapon* Weapon)
{
	OnCombatComponentWeaponFiremodeCheck.Broadcast(Weapon);
}

void UCombatComponent::Handle_OnWeaponIdle(AWeapon* Weapon)
{
	OnCombatComponentWeaponIdle.Broadcast(Weapon);
}

void UCombatComponent::Handle_OnWeaponIdleToOut(AWeapon* Weapon)
{
	OnCombatComponentWeaponIdleToOut.Broadcast(Weapon);
}

void UCombatComponent::Handle_OnWeaponMagCheck(AWeapon* Weapon)
{
	if (HasAuthority() && Weapon && Weapon == EquippedWeapon)
	{
		CombatAction = ECombatAction::CA_Idle;
	}
	OnCombatComponentWeaponMagCheck.Broadcast(Weapon);
}

void UCombatComponent::Handle_OnWeaponMagIn(AWeapon* Weapon)
{
	OnCombatComponentWeaponMagIn.Broadcast(Weapon);
}

void UCombatComponent::Handle_OnWeaponMagOut(AWeapon* Weapon)
{
	OnCombatComponentWeaponMagOut.Broadcast(Weapon);
}

void UCombatComponent::Handle_OnWeaponOut(AWeapon* Weapon)
{
	OnCombatComponentWeaponOut.Broadcast(Weapon);
}

void UCombatComponent::Handle_OnWeaponOutToIdle(AWeapon* Weapon)
{
	OnCombatComponentWeaponOutToIdle.Broadcast(Weapon);
}

void UCombatComponent::Handle_OnWeaponOutToIdleArm(AWeapon* Weapon)
{
	OnCombatComponentWeaponOutToIdleArm.Broadcast(Weapon);
}

void UCombatComponent::Handle_OnWeaponReloadCharge(AWeapon* Weapon)
{
	if (HasAuthority() && Weapon && Weapon == EquippedWeapon)
	{
		CombatAction = ECombatAction::CA_Idle;
	}
	OnCombatComponentWeaponReloadCharge.Broadcast(Weapon);
}

void UCombatComponent::OnRep_CombatAction()
{
	
}

void UCombatComponent::OnRep_EquippedWeapon(AWeapon* PrevEquippedWeapon)
{
	
}
