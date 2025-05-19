// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CharacterCombatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/CharacterInventoryComponent.h"
#include "Animation/HandsAnimInstance.h"
#include "Character/ShooterCharacter.h"
#include "Mod/Mag.h"
#include "Types/WeaponTypes.h"
#include "Weapon/Weapon.h"

UCharacterCombatComponent::UCharacterCombatComponent() :
	//bIsFiring(false),
	CombatAction(ECombatAction::CA_Idle),
	EquippedWeapon(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;

	ShooterCharacter = GetTypedOuter<AShooterCharacter>();
	
}

void UCharacterCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ShooterCharacter && ShooterCharacter->GetHandsMesh())
	{
		if (UHandsAnimInstance* HandsAnimInstance = Cast<UHandsAnimInstance>(ShooterCharacter->GetHandsMesh()->GetAnimInstance()))
		{
			HandsAnimInstance->OnHandsAnimInstanceIdle.AddDynamic(this, &UCharacterCombatComponent::Handle_OnCharacterHandsAnimInstanceIdle);
			HandsAnimInstance->OnHandsAnimInstanceIdleToOut.AddDynamic(this, &UCharacterCombatComponent::Handle_OnCharacterHandsAnimInstanceIdleToOut);
			HandsAnimInstance->OnHandsAnimInstanceOut.AddDynamic(this, &UCharacterCombatComponent::Handle_OnCharacterHandsAnimInstanceOut);
			HandsAnimInstance->OnHandsAnimInstanceOutToIdle.AddDynamic(this, &UCharacterCombatComponent::Handle_OnCharacterHandsAnimInstanceOutToIdle);
		}
	}

}

//void UCharacterCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//}

void UCharacterCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(UCharacterCombatComponent, bIsFiring);
	DOREPLIFETIME(UCharacterCombatComponent, CombatAction);
	DOREPLIFETIME(UCharacterCombatComponent, EquippedWeapon);

}

//void UCharacterCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
//{
//	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon != WeaponToEquip)
//	{
//		NextWeapon = WeaponToEquip;
//		IdleToOut();
//	}
//}
void UCharacterCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon != WeaponToEquip)
	{
		NextWeapon = WeaponToEquip;
		CombatAction = ECombatAction::CA_IdleToOut;
	}
}

//void UCharacterCombatComponent::ReloadWeapon()
//{
//	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon && EquippedWeapon->GetMag())
//	{
//		if (ShooterCharacter && ShooterCharacter->GetCharacterInventory())
//		{
//			int8 WeaponIndex = ShooterCharacter->GetCharacterInventory()->FindWeapon(EquippedWeapon);
//			if (WeaponIndex != INDEX_NONE)
//			{
//				bool bHasInventoryAmmo = ShooterCharacter->GetCharacterInventory()->GetWeaponAmmoAtIndex(WeaponIndex) > 0;
//				bool bHasMagAmmoSpace = EquippedWeapon->GetMag()->GetAmmoCapacity() - EquippedWeapon->GetMag()->GetAmmoCount() > 0;
//				if (bHasInventoryAmmo && bHasMagAmmoSpace)
//				{
//					WeaponMagOut();
//				}
//			}
//		}
//	}
//}
void UCharacterCombatComponent::ReloadWeapon()
{
	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon)
	{
		if (ShooterCharacter && ShooterCharacter->GetCharacterInventory())
		{
			int8 WeaponIndex = ShooterCharacter->GetCharacterInventory()->FindWeapon(EquippedWeapon);
			if (WeaponIndex != INDEX_NONE)
			{
				bool bHasInventoryAmmo = ShooterCharacter->GetCharacterInventory()->GetWeaponAmmoAtIndex(WeaponIndex) > 0;
				AMag* WeaponMag = EquippedWeapon->GetMag();
				bool bHasMagAmmoSpace = WeaponMag && WeaponMag->GetAmmoCapacity() - WeaponMag->GetAmmoCount() > 0;
				if (bHasInventoryAmmo && bHasMagAmmoSpace)
				{
					CombatAction = ECombatAction::CA_MagOut;
				}
			}
		}
	}
}

void UCharacterCombatComponent::ActionEnd()
{
	if (CombatAction == ECombatAction::CA_MagIn && EquippedWeapon)
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

//void UCharacterCombatComponent::OutToIdle()
//{
//	if (CombatAction == ECombatAction::CA_Out)
//	{
//		bool bHasMagAmmo = EquippedWeapon && EquippedWeapon->GetMag() && EquippedWeapon->GetMag()->GetAmmoCount() > 0;
//		bool bHasPatronInWeaponAmmo = EquippedWeapon && EquippedWeapon->GetPatronInWeaponAmmo();
//		if (EquippedWeapon && bHasMagAmmo && !bHasPatronInWeaponAmmo)
//		{
//			CombatAction = ECombatAction::CA_OutToIdleArm;
//		}
//		else
//		{
//			CombatAction = ECombatAction::CA_OutToIdle;
//		}
//	}
//}
void UCharacterCombatComponent::OutToIdle()
{
	if (CombatAction == ECombatAction::CA_Out)
	{
		AMag* WeaponMag = EquippedWeapon != nullptr ? EquippedWeapon->GetMag() : nullptr;
		bool bHasMagAmmo = WeaponMag && WeaponMag->GetAmmoCount() > 0;
		bool bHasPatronInWeaponAmmo = EquippedWeapon && EquippedWeapon->GetPatronInWeaponAmmo();
		if (EquippedWeapon && bHasMagAmmo && !bHasPatronInWeaponAmmo)
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

//void UCharacterCombatComponent::WeaponFire(bool bFire)
//{
//	bWantsToFire = bFire;
//	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon && bWantsToFire)
//	{
//		CombatAction = ECombatAction::CA_Fire;
//	}
//}
//void UCharacterCombatComponent::WeaponFire(bool bFire)
//{
//	bWantsToFire = bFire;
//	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon && bWantsToFire)
//	{
//		bool bHasPatronInWeaponAmmo = EquippedWeapon->GetPatronInWeaponAmmo() != nullptr;
//		if (bHasPatronInWeaponAmmo)
//		{
//			CombatAction = ECombatAction::CA_Fire;
//		}
//		else
//		{
//			CombatAction = ECombatAction::CA_FireDry;
//		}
//	}
//}
void UCharacterCombatComponent::WeaponFire(bool bFire)
{
	bWantsToFire = bFire;
	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon && bFire)
	{
		if (EquippedWeapon->GetPatronInWeaponAmmo())
		{
			NumRoundsFired = 0;
			CombatAction = ECombatAction::CA_Fire;
		}
		else
		{
			CombatAction = ECombatAction::CA_FireDry;
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
	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon && EquippedWeapon->GetMag())
	{
		CombatAction = ECombatAction::CA_MagCheck;
	}
}

void UCharacterCombatComponent::WeaponMagIn()
{
	if (CombatAction == ECombatAction::CA_MagOut && EquippedWeapon && EquippedWeapon->GetMag())
	{
		CombatAction = ECombatAction::CA_MagIn;
	}
}

void UCharacterCombatComponent::WeaponMagOut()
{
	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon && EquippedWeapon->GetMag())
	{
		CombatAction = ECombatAction::CA_MagOut;
	}
}

void UCharacterCombatComponent::WeaponReloadCharge()
{
	if (CombatAction == ECombatAction::CA_MagIn && EquippedWeapon)
	{
		CombatAction = ECombatAction::CA_ReloadCharge;
	}
}

void UCharacterCombatComponent::EquipNextWeapon()
{
	if (NextWeapon)
	{
		NextWeapon->bIsHolster = false;

		NextWeapon->OnWeaponActionEnd.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponActionEnd);
		NextWeapon->OnWeaponActionStart.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponActionStart);
		NextWeapon->OnWeaponChamberCheck.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponChamberCheck);
		NextWeapon->OnWeaponFire.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponFire);
		NextWeapon->OnWeaponFireDry.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponFireDry);
		NextWeapon->OnWeaponFiremode.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponFiremode);
		NextWeapon->OnWeaponFiremodeCheck.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponFiremodeCheck);
		NextWeapon->OnWeaponIdle.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponIdle);
		NextWeapon->OnWeaponIdleToOut.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponIdleToOut);
		NextWeapon->OnWeaponMagCheck.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponMagCheck);
		NextWeapon->OnWeaponMagIn.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponMagIn);
		NextWeapon->OnWeaponMagOut.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponMagOut);
		NextWeapon->OnWeaponOut.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponOut);
		NextWeapon->OnWeaponOutToIdle.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponOutToIdle);
		NextWeapon->OnWeaponOutToIdleArm.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponOutToIdleArm);
		NextWeapon->OnWeaponReloadCharge.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponReloadCharge);

		EquippedWeapon = NextWeapon;
		NextWeapon = nullptr;
	}
}

void UCharacterCombatComponent::UnequipWeapon()
{
	if (EquippedWeapon)
	{
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
}

void UCharacterCombatComponent::Handle_OnCharacterHandsAnimInstanceIdle()
{
}

//void UCharacterCombatComponent::Handle_OnCharacterHandsAnimInstanceIdleToOut()
//{
//	if (HasAuthority())
//	{
//		Out();
//	}
//}
void UCharacterCombatComponent::Handle_OnCharacterHandsAnimInstanceIdleToOut()
{
	if (HasAuthority())
	{
		CombatAction = ECombatAction::CA_Out;
	}
}

void UCharacterCombatComponent::Handle_OnCharacterHandsAnimInstanceOut()
{
	if (HasAuthority())
	{
		EquipNextWeapon();

		if (EquippedWeapon != nullptr)
		{
			OnCombatComponentEquippedWeaponChanged.Broadcast(EquippedWeapon);
		}

		OutToIdle();
	}
}

//void UCharacterCombatComponent::Handle_OnCharacterHandsAnimInstanceOutToIdle()
//{
//	if (HasAuthority())
//	{
//		Idle();
//	}
//}
void UCharacterCombatComponent::Handle_OnCharacterHandsAnimInstanceOutToIdle()
{
	if (HasAuthority())
	{
		CombatAction = ECombatAction::CA_Idle;
	}
}

//void UCharacterCombatComponent::Handle_OnWeaponActionEnd(AWeapon* Weapon)
//{
//	if (HasAuthority())
//	{
//		Idle();
//	}
//}
void UCharacterCombatComponent::Handle_OnWeaponActionEnd(AWeapon* Weapon)
{
	if (HasAuthority())
	{
		CombatAction = ECombatAction::CA_Idle;
	}
}

void UCharacterCombatComponent::Handle_OnWeaponActionStart(AWeapon* Weapon)
{
}

//void UCharacterCombatComponent::Handle_OnWeaponChamberCheck(AWeapon* Weapon)
//{
//	if (HasAuthority())
//	{
//		Idle();
//	}
void UCharacterCombatComponent::Handle_OnWeaponChamberCheck(AWeapon* Weapon)
{
	if (HasAuthority())
	{
		CombatAction = ECombatAction::CA_Idle;
	}
}

//void UCharacterCombatComponent::Handle_OnWeaponFire(AWeapon* Weapon)
//{
//	if (EquippedWeapon && EquippedWeapon == Weapon)
//	{
//		const EWeaponFiremode WeaponFiremode = EquippedWeapon->GetFiremode();
//		const bool bHasPatronInWeaponAmmo = EquippedWeapon->GetPatronInWeaponAmmo() != nullptr;
//		if (!bWantsToFire || WeaponFiremode == EWeaponFiremode::WF_SingleShot/* || !bHasPatronInWeaponAmmo*/)
//		{
//			CombatAction = ECombatAction::CA_Idle;
//		}
//	}
//	OnCombatComponentWeaponFire.Broadcast(Weapon);
//}
//void UCharacterCombatComponent::Handle_OnWeaponFire(AWeapon* Weapon)
//{
//	if (EquippedWeapon && EquippedWeapon == Weapon)
//	{
//		const EWeaponFiremode WeaponFiremode = EquippedWeapon->GetFiremode();
//		const bool bHasPatronInWeaponAmmo = EquippedWeapon->GetPatronInWeaponAmmo() != nullptr;
//		if (!bWantsToFire || WeaponFiremode == EWeaponFiremode::WF_SingleShot/* || !bHasPatronInWeaponAmmo*/)
//		{
//			CombatAction = ECombatAction::CA_Idle;
//		}
//	}
//}
//void UCharacterCombatComponent::Handle_OnWeaponFire(AWeapon* Weapon)
//{
//	if (HasAuthority() && EquippedWeapon)
//	{
//		EWeaponFiremode WeaponFiremode = EquippedWeapon->GetFiremode();
//		bool bHasPatronInWeaponAmmo = EquippedWeapon->GetPatronInWeaponAmmo() != nullptr;
//		if (!bWantsToFire || WeaponFiremode == EWeaponFiremode::WF_SingleShot)
//		{
//			CombatAction = ECombatAction::CA_Idle;
//		}
//		else if (!bHasPatronInWeaponAmmo)
//		{
//			CombatAction = ECombatAction::CA_FireDry;
//		}
//	}
//}
//void UCharacterCombatComponent::Handle_OnWeaponFire(AWeapon* Weapon)
//{
//	if (HasAuthority() && Weapon)
//	{
//		if (!bWantsToFire || Weapon->GetFiremode() == EWeaponFiremode::WF_SingleShot)
//		{
//			CombatAction = ECombatAction::CA_Idle;
//		}
//		else if (Weapon->GetPatronInWeaponAmmo() == nullptr)
//		{
//			CombatAction = ECombatAction::CA_FireDry;
//		}
//	}
//}
void UCharacterCombatComponent::Handle_OnWeaponFire(AWeapon* Weapon)
{
	if (HasAuthority() && Weapon)
	{
		EWeaponFiremode WeaponFiremode = Weapon->GetFiremode();
		bool bFiredRoundsLimitReached = WeaponFiremode == (EWeaponFiremode)++NumRoundsFired;

		if ((!bWantsToFire && WeaponFiremode == EWeaponFiremode::WF_FullAuto) || bFiredRoundsLimitReached)
		{
			CombatAction = ECombatAction::CA_Idle;
		}
		else if (Weapon->GetPatronInWeaponAmmo() == nullptr)
		{
			CombatAction = ECombatAction::CA_FireDry;
		}
	}
}

//void UCharacterCombatComponent::Handle_OnWeaponFireDry(AWeapon* Weapon)
//{
//	//if (Weapon && Weapon == EquippedWeapon)
//	//{
//	//	//bIsFiring = false;
//	//	CombatAction = ECombatAction::CA_Idle;
//	//}
//	//OnCombatComponentWeaponFireDry.Broadcast(Weapon);
//	if (EquippedWeapon && EquippedWeapon == Weapon)
//	{
//		CombatAction = ECombatAction::CA_Idle;
//	}
//	OnCombatComponentWeaponFireDry.Broadcast(Weapon);
//}
//void UCharacterCombatComponent::Handle_OnWeaponFireDry(AWeapon* Weapon)
//{
//	//if (Weapon && Weapon == EquippedWeapon)
//	//{
//	//	//bIsFiring = false;
//	//	CombatAction = ECombatAction::CA_Idle;
//	//}
//	//OnCombatComponentWeaponFireDry.Broadcast(Weapon);
//	if (EquippedWeapon && EquippedWeapon == Weapon)
//	{
//		CombatAction = ECombatAction::CA_Idle;
//	}
//}
//void UCharacterCombatComponent::Handle_OnWeaponFireDry(AWeapon* Weapon)
//{
//	if (HasAuthority())
//	{
//		Idle();
//	}
//}
void UCharacterCombatComponent::Handle_OnWeaponFireDry(AWeapon* Weapon)
{
	if (HasAuthority())
	{
		CombatAction = ECombatAction::CA_Idle;
	}
}

//void UCharacterCombatComponent::Handle_OnWeaponFiremode(AWeapon* Weapon)
//{
//	if (HasAuthority())
//	{
//		Idle();
//	}
//}
void UCharacterCombatComponent::Handle_OnWeaponFiremode(AWeapon* Weapon)
{
	if (HasAuthority())
	{
		CombatAction = ECombatAction::CA_Idle;
	}
}

//void UCharacterCombatComponent::Handle_OnWeaponFiremodeCheck(AWeapon* Weapon)
//{
//	if (HasAuthority())
//	{
//		Idle();
//	}
//}
void UCharacterCombatComponent::Handle_OnWeaponFiremodeCheck(AWeapon* Weapon)
{
	if (HasAuthority())
	{
		CombatAction = ECombatAction::CA_Idle;
	}
}

void UCharacterCombatComponent::Handle_OnWeaponIdle(AWeapon* Weapon)
{
}

//void UCharacterCombatComponent::Handle_OnWeaponIdleToOut(AWeapon* Weapon)
//{
//	if (HasAuthority())
//	{
//		Out();
//	}
//}
void UCharacterCombatComponent::Handle_OnWeaponIdleToOut(AWeapon* Weapon)
{
	if (HasAuthority())
	{
		CombatAction = ECombatAction::CA_Out;
	}
}

//void UCharacterCombatComponent::Handle_OnWeaponMagCheck(AWeapon* Weapon)
//{
//	if (HasAuthority())
//	{
//		Idle();
//	}
//}
void UCharacterCombatComponent::Handle_OnWeaponMagCheck(AWeapon* Weapon)
{
	if (HasAuthority())
	{
		CombatAction = ECombatAction::CA_Idle;
	}
}

//void UCharacterCombatComponent::Handle_OnWeaponMagIn(AWeapon* Weapon)
//{
//	if (HasAuthority())
//	{
//		bool bNeedReloadCharge = EquippedWeapon && EquippedWeapon->GetMag() && EquippedWeapon->GetMag()->GetAmmoCount() > 0 && EquippedWeapon->GetPatronInWeaponAmmo() == nullptr;
//		if (bNeedReloadCharge)
//		{
//			WeaponReloadCharge();
//		}
//		else
//		{
//			ActionEnd();
//		}
//	}
//}
void UCharacterCombatComponent::Handle_OnWeaponMagIn(AWeapon* Weapon)
{
	if (HasAuthority())
	{
		AMag* WeaponMag = Weapon != nullptr ? Weapon->GetMag() : nullptr;
		bool bHasMagAmmo = WeaponMag && WeaponMag->GetAmmoCount() > 0;
		bool bHasPatronInWeaponAmmo = Weapon && Weapon->GetPatronInWeaponAmmo();
		if (bHasMagAmmo && !bHasPatronInWeaponAmmo)
		{
			CombatAction = ECombatAction::CA_ReloadCharge;
		}
		else
		{
			CombatAction = ECombatAction::CA_ActionEnd;
		}
	}
}

//void UCharacterCombatComponent::Handle_OnWeaponMagOut(AWeapon* Weapon)
//{
//	if (HasAuthority())
//	{
//		if (ShooterCharacter && ShooterCharacter->GetCharacterInventory())
//		{
//			int8 WeaponIndex = ShooterCharacter->GetCharacterInventory()->FindWeapon(EquippedWeapon);
//			if (WeaponIndex != INDEX_NONE)
//			{
//				ShooterCharacter->GetCharacterInventory()->LoadAmmoInWeaponMag(WeaponIndex);
//			}
//		}
//		WeaponMagIn();
//	}
//}
void UCharacterCombatComponent::Handle_OnWeaponMagOut(AWeapon* Weapon)
{
	if (HasAuthority())
	{
		if (ShooterCharacter && ShooterCharacter->GetCharacterInventory())
		{
			int8 WeaponIndex = ShooterCharacter->GetCharacterInventory()->FindWeapon(Weapon);
			if (WeaponIndex != INDEX_NONE)
			{
				ShooterCharacter->GetCharacterInventory()->LoadAmmoInWeaponMag(WeaponIndex);
			}
		}
		CombatAction = ECombatAction::CA_MagIn;
	}
}

//void UCharacterCombatComponent::Handle_OnWeaponOut(AWeapon* Weapon)
//{
//	OnCombatComponentWeaponOut.Broadcast(Weapon);
//}
void UCharacterCombatComponent::Handle_OnWeaponOut(AWeapon* Weapon)
{
	if (HasAuthority())
	{
		UnequipWeapon();

		EquipNextWeapon();

		if (EquippedWeapon != Weapon)
		{
			OnCombatComponentEquippedWeaponChanged.Broadcast(EquippedWeapon);
		}

		OutToIdle();
	}
}

//void UCharacterCombatComponent::Handle_OnWeaponOutToIdle(AWeapon* Weapon)
//{
//	if (HasAuthority())
//	{
//		Idle();
//	}
//}
void UCharacterCombatComponent::Handle_OnWeaponOutToIdle(AWeapon* Weapon)
{
	if (HasAuthority())
	{
		CombatAction = ECombatAction::CA_Idle;
	}
}

//void UCharacterCombatComponent::Handle_OnWeaponOutToIdleArm(AWeapon* Weapon)
//{
//	if (HasAuthority())
//	{
//		Idle();
//	}
//}
void UCharacterCombatComponent::Handle_OnWeaponOutToIdleArm(AWeapon* Weapon)
{
	if (HasAuthority())
	{
		CombatAction = ECombatAction::CA_Idle;
	}
}

//void UCharacterCombatComponent::Handle_OnWeaponReloadCharge(AWeapon* Weapon)
//{
//	if (HasAuthority())
//	{
//		Idle();
//	}
//}
void UCharacterCombatComponent::Handle_OnWeaponReloadCharge(AWeapon* Weapon)
{
	if (HasAuthority())
	{
		CombatAction = ECombatAction::CA_Idle;
	}
}

void UCharacterCombatComponent::OnRep_CombatAction()
{
}

void UCharacterCombatComponent::OnRep_EquippedWeapon(AWeapon* PrevEquippedWeapon)
{
	OnCombatComponentEquippedWeaponChanged.Broadcast(EquippedWeapon);
}
