// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CharacterCombatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/CharacterInventoryComponent.h"
#include "Animation/HandsAnimInstance.h"
#include "Animation/WeaponAnimInstance.h"
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

void UCharacterCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (EquippedWeapon != WeaponToEquip && GetAnimCombatAction() == ECombatAction::CA_Idle)
	{
		NextWeapon = WeaponToEquip;
		CombatAction = ECombatAction::CA_IdleToOut;
	}
}

void UCharacterCombatComponent::ReloadWeapon()
{
	if (EquippedWeapon && GetWeaponAnimCombatAction() == ECombatAction::CA_Idle)
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

void UCharacterCombatComponent::CheckWeaponChamber()
{
	if (EquippedWeapon && GetWeaponAnimCombatAction() == ECombatAction::CA_Idle)
	{
		CombatAction = ECombatAction::CA_ChamberCheck;
	}
}

void UCharacterCombatComponent::FireWeapon(bool bFire)
{
	bWantsToFire = bFire;
	if (EquippedWeapon && GetWeaponAnimCombatAction() == ECombatAction::CA_Idle && bFire)
	{
		if (EquippedWeapon->GetAmmoInWeapon())
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

void UCharacterCombatComponent::ChangeWeaponFiremode()
{
	if (EquippedWeapon && GetWeaponAnimCombatAction() == ECombatAction::CA_Idle && EquippedWeapon->GetNumFiremodes() > 1)
	{
		CombatAction = ECombatAction::CA_Firemode;
	}
}

void UCharacterCombatComponent::CheckWeaponFiremode()
{
	if (EquippedWeapon && GetWeaponAnimCombatAction() == ECombatAction::CA_Idle && EquippedWeapon->GetNumFiremodes() > 1)
	{
		CombatAction = ECombatAction::CA_FiremodeCheck;
	}
}

void UCharacterCombatComponent::CheckWeaponMag()
{
	if (EquippedWeapon && GetWeaponAnimCombatAction() == ECombatAction::CA_Idle && EquippedWeapon->GetMag())
	{
		CombatAction = ECombatAction::CA_MagCheck;
	}
}

ECombatAction UCharacterCombatComponent::GetWeaponAnimCombatAction() const
{
	if (EquippedWeapon && EquippedWeapon->GetMesh())
	{
		if (UWeaponAnimInstance* WeaponAnimInstance = Cast<UWeaponAnimInstance>(EquippedWeapon->GetMesh()->GetAnimInstance()))
		{
			return WeaponAnimInstance->GetCombatAction();
		}
	}
	return ECombatAction::CA_None;
}

ECombatAction UCharacterCombatComponent::GetHandsAnimCombatAction() const
{
	if (ShooterCharacter && ShooterCharacter->GetHandsMesh())
	{
		if (UHandsAnimInstance* HandsAnimInstance = Cast<UHandsAnimInstance>(ShooterCharacter->GetHandsMesh()->GetAnimInstance()))
		{
			return HandsAnimInstance->GetCombatAction();
		}
	}
	return ECombatAction::CA_None;
}

ECombatAction UCharacterCombatComponent::GetAnimCombatAction() const
{
	if (EquippedWeapon)
	{
		return GetWeaponAnimCombatAction();
	}
	else
	{
		return GetHandsAnimCombatAction();
	}
}

//void UCharacterCombatComponent::ActionEnd()
//{
//	if (CombatAction == ECombatAction::CA_MagIn && EquippedWeapon)
//	{
//		CombatAction = ECombatAction::CA_ActionEnd;
//	}
//}

//void UCharacterCombatComponent::ActionStart()
//{
//}

//void UCharacterCombatComponent::Idle()
//{
//	CombatAction = ECombatAction::CA_Idle;
//}

//void UCharacterCombatComponent::IdleToOut()
//{
//	if (CombatAction == ECombatAction::CA_Idle)
//	{
//		CombatAction = ECombatAction::CA_IdleToOut;
//	}
//}

//void UCharacterCombatComponent::Out()
//{
//	if (CombatAction == ECombatAction::CA_IdleToOut)
//	{
//		CombatAction = ECombatAction::CA_Out;
//	}
//}

void UCharacterCombatComponent::OutToIdle()
{
	if (GetAnimCombatAction() == ECombatAction::CA_Out)
	{
		AMag* WeaponMag = EquippedWeapon != nullptr ? EquippedWeapon->GetMag() : nullptr;
		bool bHasMagAmmo = WeaponMag && WeaponMag->GetAmmoCount() > 0;
		bool bHasAmmoInWeapon = EquippedWeapon && EquippedWeapon->GetAmmoInWeapon();
		if (EquippedWeapon && bHasMagAmmo && !bHasAmmoInWeapon)
		{
			CombatAction = ECombatAction::CA_OutToIdleArm;
		}
		else
		{
			CombatAction = ECombatAction::CA_OutToIdle;
		}
	}
}

//void UCharacterCombatComponent::WeaponMagIn()
//{
//	if (CombatAction == ECombatAction::CA_MagOut && EquippedWeapon && EquippedWeapon->GetMag())
//	{
//		CombatAction = ECombatAction::CA_MagIn;
//	}
//}

//void UCharacterCombatComponent::WeaponMagOut()
//{
//	if (CombatAction == ECombatAction::CA_Idle && EquippedWeapon && EquippedWeapon->GetMag())
//	{
//		CombatAction = ECombatAction::CA_MagOut;
//	}
//}

//void UCharacterCombatComponent::WeaponReloadCharge()
//{
//	if (CombatAction == ECombatAction::CA_MagIn && EquippedWeapon)
//	{
//		CombatAction = ECombatAction::CA_ReloadCharge;
//	}
//}

void UCharacterCombatComponent::AddDelegates(AWeapon* Weapon)
{
	if (Weapon)
	{
		Weapon->OnWeaponActionEnd.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponActionEnd);
		Weapon->OnWeaponActionStart.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponActionStart);
		Weapon->OnWeaponChamberCheck.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponChamberCheck);
		Weapon->OnWeaponFire.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponFire);
		Weapon->OnWeaponFireDry.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponFireDry);
		Weapon->OnWeaponFiremode.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponFiremode);
		Weapon->OnWeaponFiremodeCheck.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponFiremodeCheck);
		Weapon->OnWeaponIdle.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponIdle);
		Weapon->OnWeaponIdleToOut.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponIdleToOut);
		Weapon->OnWeaponMagCheck.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponMagCheck);
		Weapon->OnWeaponMagIn.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponMagIn);
		Weapon->OnWeaponMagOut.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponMagOut);
		Weapon->OnWeaponOut.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponOut);
		Weapon->OnWeaponOutToIdle.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponOutToIdle);
		Weapon->OnWeaponOutToIdleArm.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponOutToIdleArm);
		Weapon->OnWeaponReloadCharge.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponReloadCharge);
	}
}

void UCharacterCombatComponent::RemoveDelegates(AWeapon* Weapon)
{
	if (Weapon)
	{
		Weapon->OnWeaponActionEnd.RemoveAll(this);
		Weapon->OnWeaponActionStart.RemoveAll(this);
		Weapon->OnWeaponChamberCheck.RemoveAll(this);
		Weapon->OnWeaponFire.RemoveAll(this);
		Weapon->OnWeaponFireDry.RemoveAll(this);
		Weapon->OnWeaponFiremode.RemoveAll(this);
		Weapon->OnWeaponFiremodeCheck.RemoveAll(this);
		Weapon->OnWeaponIdle.RemoveAll(this);
		Weapon->OnWeaponIdleToOut.RemoveAll(this);
		Weapon->OnWeaponMagCheck.RemoveAll(this);
		Weapon->OnWeaponMagIn.RemoveAll(this);
		Weapon->OnWeaponMagOut.RemoveAll(this);
		Weapon->OnWeaponOut.RemoveAll(this);
		Weapon->OnWeaponOutToIdle.RemoveAll(this);
		Weapon->OnWeaponOutToIdleArm.RemoveAll(this);
		Weapon->OnWeaponReloadCharge.RemoveAll(this);
	}
}

void UCharacterCombatComponent::Handle_OnCharacterHandsAnimInstanceIdle()
{
}

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
		EquippedWeapon = NextWeapon;
		NextWeapon = nullptr;

		if (EquippedWeapon)
		{
			EquippedWeapon->bIsHolster = false;
			AddDelegates(EquippedWeapon);
		}

		if (EquippedWeapon != nullptr)
		{
			OnCombatComponentEquippedWeaponChanged.Broadcast(EquippedWeapon);
		}
		
		OutToIdle();
	}
}

void UCharacterCombatComponent::Handle_OnCharacterHandsAnimInstanceOutToIdle()
{
	if (HasAuthority())
	{
		CombatAction = ECombatAction::CA_Idle;
	}
}

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

void UCharacterCombatComponent::Handle_OnWeaponChamberCheck(AWeapon* Weapon)
{
	if (HasAuthority())
	{
		CombatAction = ECombatAction::CA_Idle;
	}
}

void UCharacterCombatComponent::Handle_OnWeaponFire(AWeapon* Weapon)
{
	if (HasAuthority())
	{
		if (Weapon)
		{
			++NumRoundsFired;

			EWeaponFiremode WeaponFiremode = Weapon->GetFiremode();
			bool bFiredRoundsLimitReached = (WeaponFiremode == EWeaponFiremode::WF_SingleShot && NumRoundsFired == 1) ||
				(WeaponFiremode == EWeaponFiremode::WF_2RoundsBurst && NumRoundsFired == 2) ||
				(WeaponFiremode == EWeaponFiremode::WF_3RoundsBurst && NumRoundsFired == 3);

			if ((!bWantsToFire && WeaponFiremode == EWeaponFiremode::WF_FullAuto) || bFiredRoundsLimitReached)
			{
				CombatAction = ECombatAction::CA_Idle;
			}
			else if (Weapon->GetAmmoInWeapon() == nullptr)
			{
				CombatAction = ECombatAction::CA_FireDry;
			}
		}
	}
}

void UCharacterCombatComponent::Handle_OnWeaponFireDry(AWeapon* Weapon)
{
	if (HasAuthority())
	{
		CombatAction = ECombatAction::CA_Idle;
	}
}

void UCharacterCombatComponent::Handle_OnWeaponFiremode(AWeapon* Weapon)
{
	if (HasAuthority())
	{
		CombatAction = ECombatAction::CA_Idle;
	}
}

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

void UCharacterCombatComponent::Handle_OnWeaponIdleToOut(AWeapon* Weapon)
{
	if (HasAuthority())
	{
		CombatAction = ECombatAction::CA_Out;
	}
}

void UCharacterCombatComponent::Handle_OnWeaponMagCheck(AWeapon* Weapon)
{
	if (HasAuthority())
	{
		CombatAction = ECombatAction::CA_Idle;
	}
}

void UCharacterCombatComponent::Handle_OnWeaponMagIn(AWeapon* Weapon)
{
	if (HasAuthority())
	{
		AMag* WeaponMag = Weapon != nullptr ? Weapon->GetMag() : nullptr;
		bool bHasMagAmmo = WeaponMag && WeaponMag->GetAmmoCount() > 0;
		bool bHasAmmoInWeapon = Weapon && Weapon->GetAmmoInWeapon();
		if (bHasMagAmmo && !bHasAmmoInWeapon)
		{
			CombatAction = ECombatAction::CA_ReloadCharge;
		}
		else
		{
			CombatAction = ECombatAction::CA_ActionEnd;
		}
	}
}

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

void UCharacterCombatComponent::Handle_OnWeaponOut(AWeapon* Weapon)
{
	if (HasAuthority())
	{
		if (Weapon)
		{
			Weapon->bIsHolster = true;
			RemoveDelegates(Weapon);
		}

		EquippedWeapon = NextWeapon;
		NextWeapon = nullptr;

		if (EquippedWeapon)
		{
			EquippedWeapon->bIsHolster = false;
			AddDelegates(EquippedWeapon);
		}

		if (EquippedWeapon != Weapon)
		{
			OnCombatComponentEquippedWeaponChanged.Broadcast(EquippedWeapon);
		}

		OutToIdle();
	}
}

void UCharacterCombatComponent::Handle_OnWeaponOutToIdle(AWeapon* Weapon)
{
	if (HasAuthority())
	{
		CombatAction = ECombatAction::CA_Idle;
	}
}

void UCharacterCombatComponent::Handle_OnWeaponOutToIdleArm(AWeapon* Weapon)
{
	if (HasAuthority())
	{
		CombatAction = ECombatAction::CA_Idle;
	}
}

void UCharacterCombatComponent::Handle_OnWeaponReloadCharge(AWeapon* Weapon)
{
	if (HasAuthority())
	{
		CombatAction = ECombatAction::CA_Idle;
	}
}

void UCharacterCombatComponent::OnRep_CombatAction()
{
	UE_LOG(LogTemp, Error, TEXT(__FUNCTION__));
}

void UCharacterCombatComponent::OnRep_EquippedWeapon(AWeapon* PrevEquippedWeapon)
{
	UE_LOG(LogTemp, Error, TEXT(__FUNCTION__));
	
	if (PrevEquippedWeapon)
	{
		RemoveDelegates(PrevEquippedWeapon);
	}

	if (EquippedWeapon)
	{
		AddDelegates(EquippedWeapon);
	}

	OnCombatComponentEquippedWeaponChanged.Broadcast(EquippedWeapon);	
}
