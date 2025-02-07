// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/CombatComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "Actor/Weapon.h"
#include "Character/ShooterCharacter.h"
#include "Enum/WeaponFiremode.h"

UCombatComponent::UCombatComponent() :
	bIsAiming(false),
	bIsFiring(false),
	CombatAction(ECombatAction::CA_Idle),
	EquippedWeapon(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;

	ADSTime = 1.0f;

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, bIsAiming);
	DOREPLIFETIME(UCombatComponent, bIsFiring);
	DOREPLIFETIME(UCombatComponent, CombatAction);
	DOREPLIFETIME(UCombatComponent, EquippedWeapon);

}

void UCombatComponent::Server_ActionEnd_Implementation()
{
	if (CombatAction == ECombatAction::CA_MagIn)
	{
		Server_SetCombatAction(ECombatAction::CA_ActionEnd);
	}
}

void UCombatComponent::Server_ActionStart_Implementation()
{
}

void UCombatComponent::Server_EquipWeapon_Implementation(AWeapon* WeaponToEquip, USkeletalMeshComponent* ParentSkeletalMesh, FName InParentSocketName)
{
	if (WeaponToEquip == nullptr || EquippedWeapon)
	{
		return;
	}
	WeaponToEquip->AttachToComponent(ParentSkeletalMesh, FAttachmentTransformRules::KeepRelativeTransform, InParentSocketName);
	WeaponToEquip->Server_SetIsHolster(false);

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

void UCombatComponent::Server_Idle_Implementation()
{
	Server_SetCombatAction(ECombatAction::CA_Idle);
}

void UCombatComponent::Server_IdleToOut_Implementation()
{
	if (CombatAction == ECombatAction::CA_Idle)
	{
		Server_SetCombatAction(ECombatAction::CA_IdleToOut);
	}
}

void UCombatComponent::Server_Out_Implementation()
{
	if (CombatAction == ECombatAction::CA_IdleToOut)
	{
		Server_SetCombatAction(ECombatAction::CA_Out);
	}
}

void UCombatComponent::Server_OutToIdle_Implementation()
{
	if (CombatAction == ECombatAction::CA_Out)
	{
		Server_SetCombatAction(EquippedWeapon && EquippedWeapon->DoesNeedCharge() ? ECombatAction::CA_OutToIdleArm : ECombatAction::CA_OutToIdle);
	}
}

void UCombatComponent::Server_UnequipWeapon_Implementation(USkeletalMeshComponent* ParentSkeletalMesh, FName InParentSocketName)
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	EquippedWeapon->AttachToComponent(ParentSkeletalMesh, FAttachmentTransformRules::KeepRelativeTransform, InParentSocketName);
	EquippedWeapon->Server_SetIsHolster(true);

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

void UCombatComponent::Server_WeaponChamberCheck_Implementation()
{
	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon)
	{
		Server_SetCombatAction(ECombatAction::CA_ChamberCheck);
	}
}

void UCombatComponent::Server_WeaponFire_Implementation(const bool bFiring)
{
	if (EquippedWeapon)
	{
		if (CombatAction == ECombatAction::CA_Idle && bFiring)
		{
			Server_SetIsFiring(true);
			Server_SetCombatAction(ECombatAction::CA_Fire);
		}
		else if (!bFiring)
		{
			Server_SetIsFiring(false);
		}
	}
}

void UCombatComponent::Server_WeaponFiremode_Implementation()
{
	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon && EquippedWeapon->HasFiremodes())
	{
		Server_SetCombatAction(ECombatAction::CA_Firemode);
	}
}

void UCombatComponent::Server_WeaponFiremodeCheck_Implementation()
{
	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon && EquippedWeapon->HasFiremodes())
	{
		Server_SetCombatAction(ECombatAction::CA_FiremodeCheck);
	}
}

void UCombatComponent::Server_WeaponMagCheck_Implementation()
{
	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon && EquippedWeapon->HasMag())
	{
		Server_SetCombatAction(ECombatAction::CA_MagCheck);
	}
}

void UCombatComponent::Server_WeaponMagIn_Implementation()
{
	if (CombatAction == ECombatAction::CA_MagOut && EquippedWeapon && EquippedWeapon->HasMag())
	{
		Server_SetCombatAction(ECombatAction::CA_MagIn);
	}
}

void UCombatComponent::Server_WeaponMagOut_Implementation()
{
	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon && EquippedWeapon->HasMag())
	{
		Server_SetCombatAction(ECombatAction::CA_MagOut);
	}
}

void UCombatComponent::Server_WeaponReloadCharge_Implementation()
{
	if (CombatAction == ECombatAction::CA_MagIn && EquippedWeapon && EquippedWeapon->DoesNeedCharge())
	{
		Server_SetCombatAction(ECombatAction::CA_ReloadCharge);
	}
}

void UCombatComponent::SetIsAiming(bool bAiming)
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	bIsAiming = bAiming;
	if (!HasAuthority())
	{
		Server_SetIsAiming(bAiming);
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UCombatComponent::Handle_OnWeaponActionEnd(AWeapon* Weapon)
{
	if (HasAuthority() && Weapon && Weapon == EquippedWeapon)
	{
		Server_SetCombatAction(ECombatAction::CA_Idle);
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
		Server_SetCombatAction(ECombatAction::CA_Idle);
	}
	OnCombatComponentWeaponChamberCheck.Broadcast(Weapon);
}

void UCombatComponent::Handle_OnWeaponFire(AWeapon* Weapon)
{
	if (Weapon && Weapon == EquippedWeapon)
	{
		EWeaponFiremode WeaponFiremode = Weapon->GetFiremode();
		if (WeaponFiremode == EWeaponFiremode::WF_SingleShot || !Weapon->HasPatronInWeaponAmmo())
		{
			Server_SetIsFiring(false);
			Server_SetCombatAction(ECombatAction::CA_Idle);
		}
		if (!bIsFiring)
		{
			Server_SetCombatAction(ECombatAction::CA_Idle);
		}
	}
	OnCombatComponentWeaponFire.Broadcast(Weapon);
}

void UCombatComponent::Handle_OnWeaponFireDry(AWeapon* Weapon)
{
	if (Weapon && Weapon == EquippedWeapon)
	{
		Server_SetIsFiring(false);
		Server_SetCombatAction(ECombatAction::CA_Idle);
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
		Server_SetCombatAction(ECombatAction::CA_Idle);
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
		Server_SetCombatAction(ECombatAction::CA_Idle);
	}
	OnCombatComponentWeaponReloadCharge.Broadcast(Weapon);
}

void UCombatComponent::OnRep_CombatAction()
{
	
}

void UCombatComponent::OnRep_EquippedWeapon(AWeapon* PrevEquippedWeapon)
{
	
}

void UCombatComponent::Server_SetCombatAction_Implementation(ECombatAction Action)
{
	CombatAction = Action;
}

void UCombatComponent::Server_SetIsAiming_Implementation(bool bAiming)
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	bIsAiming = bAiming;
}

void UCombatComponent::Server_SetIsFiring_Implementation(const bool bFiring)
{
	bIsFiring = bFiring;
}
