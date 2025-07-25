// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CharacterCombatComponent.h"
#include "Animation/AnimNode_StateMachine.h"
#include "Net/UnrealNetwork.h"
#include "Components/CharacterInventoryComponent.h"
#include "Animation/HandsAnimInstance.h"
#include "Animation/WeaponAnimInstance.h"
#include "Character/ShooterCharacter.h"
#include "Mod/Mag.h"
#include "Types/CombatTypes.h"
#include "Types/WeaponTypes.h"
#include "Weapon/Weapon.h"

UCharacterCombatComponent::UCharacterCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	ShooterCharacter = GetTypedOuter<AShooterCharacter>();
	CombatAction = ECombatAction::CA_Idle;
	
}

void UCharacterCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ShooterCharacter)
	{
		if (UHandsAnimInstance* HandsAnimInstance = Cast<UHandsAnimInstance>(ShooterCharacter->GetHandsAnimInstance()))
		{
			HandsAnimInstance->OnHandsAnimInstanceIdle.AddDynamic(this, &UCharacterCombatComponent::Handle_OnCharacterHandsAnimInstanceIdle);
			HandsAnimInstance->OnHandsAnimInstanceIdleToOut.AddDynamic(this, &UCharacterCombatComponent::Handle_OnCharacterHandsAnimInstanceIdleToOut);
			HandsAnimInstance->OnHandsAnimInstanceOut.AddDynamic(this, &UCharacterCombatComponent::Handle_OnCharacterHandsAnimInstanceOut);
			HandsAnimInstance->OnHandsAnimInstanceOutToIdle.AddDynamic(this, &UCharacterCombatComponent::Handle_OnCharacterHandsAnimInstanceOutToIdle);
		}
	}

}

void UCharacterCombatComponent::SetCombatAction(ECombatAction NewCombatAction, bool bUpdateWeaponAnimCombatAction)
{
	if (CombatAction != NewCombatAction)
	{
		CombatAction = NewCombatAction;
		OnCharacterCombatCombatActionChanged.Broadcast(CombatAction);

		// Immidiate CombatAction update in WeaponAnimInstance so we can guard against anim notify broadcasts
		if (bUpdateWeaponAnimCombatAction)
		{
			UpdateWeaponAnimCombatAction();
		}
	}
}

void UCharacterCombatComponent::SetEquippedWeapon(AWeapon* Weapon)
{
	if (EquippedWeapon != Weapon)
	{
		RemoveDelegates(EquippedWeapon);
		EquippedWeapon = Weapon;
		AddDelegates(EquippedWeapon);

		OnCharacterCombatEquippedWeaponChanged.Broadcast(EquippedWeapon);
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

	DOREPLIFETIME(UCharacterCombatComponent, CombatAction);
	DOREPLIFETIME(UCharacterCombatComponent, EquippedWeapon);

}

bool UCharacterCombatComponent::CanEquipWeapon(AWeapon* WeaponToEquip)
{
	return EquippedWeapon != WeaponToEquip && GetRelevantAnimCombatAction() == ECombatAction::CA_Idle;
}

void UCharacterCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (CanEquipWeapon(WeaponToEquip))
	{
		Server_EquipWeapon(WeaponToEquip);
	}
}

bool UCharacterCombatComponent::CanReloadWeapon()
{
	if (EquippedWeapon && GetWeaponAnimCombatAction() == ECombatAction::CA_Idle)
	{
		if (ShooterCharacter && ShooterCharacter->GetCharacterInventory())
		{
			if (ShooterCharacter->GetCharacterInventory()->GetWeaponAmmo(EquippedWeapon) > 0)
			{
				if (AMag* WeaponMag = EquippedWeapon->GetMag())
				{
					if (WeaponMag->GetAmmoSpace() > 0)
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

void UCharacterCombatComponent::ReloadWeapon()
{
	if (CanReloadWeapon())
	{
		Server_ReloadWeapon();
	}
}

bool UCharacterCombatComponent::CanCheckWeaponChamber()
{
	return EquippedWeapon && GetWeaponAnimCombatAction() == ECombatAction::CA_Idle;
}

void UCharacterCombatComponent::CheckWeaponChamber()
{
	if (CanCheckWeaponChamber())
	{
		Server_CheckWeaponChamber();
	}
}

bool UCharacterCombatComponent::CanFireWeapon()
{
	return EquippedWeapon && GetWeaponAnimCombatAction() == ECombatAction::CA_Idle;
}

void UCharacterCombatComponent::FireWeapon(bool bFire)
{
	Server_FireWeapon(bFire);

	if (GetOwnerRole() != ROLE_Authority)
	{
		bWantsToFire = bFire;
		if (CanFireWeapon() && bFire)
		{
			Fire();
		}
	}
}

bool UCharacterCombatComponent::CanChangeWeaponFiremode()
{
	return EquippedWeapon && GetWeaponAnimCombatAction() == ECombatAction::CA_Idle && EquippedWeapon->GetNumFiremodes() > 1;
}

void UCharacterCombatComponent::ChangeWeaponFiremode()
{
	if (CanChangeWeaponFiremode())
	{
		Server_ChangeWeaponFiremode();
	}
}

bool UCharacterCombatComponent::CanCheckWeaponFiremode()
{
	return EquippedWeapon && GetWeaponAnimCombatAction() == ECombatAction::CA_Idle && EquippedWeapon->GetNumFiremodes() > 1;
}

void UCharacterCombatComponent::CheckWeaponFiremode()
{
	if (CanCheckWeaponFiremode())
	{
		Server_CheckWeaponFiremode();
	}
}

bool UCharacterCombatComponent::CanCheckWeaponMag()
{
	return EquippedWeapon && GetWeaponAnimCombatAction() == ECombatAction::CA_Idle && EquippedWeapon->GetMag();
}

void UCharacterCombatComponent::CheckWeaponMag()
{
	if (CanCheckWeaponMag())
	{
		Server_CheckWeaponMag();
	}
}

ECombatAction UCharacterCombatComponent::GetWeaponAnimCombatAction() const
{
	return EquippedWeapon != nullptr ? EquippedWeapon->GetWeaponAnimCombatAction() : ECombatAction::CA_None;
}

ECombatAction UCharacterCombatComponent::GetHandsAnimCombatAction() const
{
	return ShooterCharacter != nullptr ? ShooterCharacter->GetHandsAnimCombatAction() : ECombatAction::CA_None;
}

ECombatAction UCharacterCombatComponent::GetRelevantAnimCombatAction() const
{
	return EquippedWeapon != nullptr ? GetWeaponAnimCombatAction() : GetHandsAnimCombatAction();
}

void UCharacterCombatComponent::UpdateWeaponAnimCombatAction() const
{
	if (EquippedWeapon)
	{
		EquippedWeapon->SetWeaponAnimCombatAction(CombatAction);
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

void UCharacterCombatComponent::Fire()
{
	if (EquippedWeapon)
	{
		if (EquippedWeapon->GetPatronInWeaponAmmo() != nullptr)
		{
			NumRoundsFired = 0;
			SetCombatAction(ECombatAction::CA_Fire);
		}
		else
		{
			SetCombatAction(ECombatAction::CA_FireDry);
		}
	}
}

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
	if (GetRelevantAnimCombatAction() == ECombatAction::CA_Out)
	{
		AMag* WeaponMag = EquippedWeapon != nullptr ? EquippedWeapon->GetMag() : nullptr;
		bool bHasMagAmmo = WeaponMag && WeaponMag->GetAmmoCount() > 0;
		bool bHasPatronInWeaponAmmo = EquippedWeapon && EquippedWeapon->GetPatronInWeaponAmmo() != nullptr;
		if (EquippedWeapon && bHasMagAmmo && !bHasPatronInWeaponAmmo)
		{
			SetCombatAction(ECombatAction::CA_OutToIdleArm);
		}
		else
		{
			SetCombatAction(ECombatAction::CA_OutToIdle);
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
		Weapon->OnWeaponHandsInHalf1.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponHandsInHalf1);
		Weapon->OnWeaponHandsInHalf2.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponHandsInHalf2);
		Weapon->OnWeaponHandsOutHalf1.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponHandsOutHalf1);
		Weapon->OnWeaponHandsOutHalf2.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponHandsOutHalf2);
		Weapon->OnWeaponIdle.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponIdle);
		Weapon->OnWeaponIdleToOut.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponIdleToOut);
		Weapon->OnWeaponMagCheck.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponMagCheck);
		Weapon->OnWeaponMagIn.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponMagIn);
		Weapon->OnWeaponMagOut.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponMagOut);
		Weapon->OnWeaponOut.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponOut);
		Weapon->OnWeaponOutToIdle.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponOutToIdle);
		Weapon->OnWeaponOutToIdleArm.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponOutToIdleArm);
		Weapon->OnWeaponReloadCatch.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponReloadCatch);
		Weapon->OnWeaponReloadCharge.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponReloadCharge);
		Weapon->OnWeaponRecoilGenerated.AddDynamic(this, &UCharacterCombatComponent::Handle_OnWeaponRecoilGenerated);
	}
}

void UCharacterCombatComponent::RemoveDelegates(AWeapon* Weapon)
{
	if (Weapon)
	{
		Weapon->OnWeaponActionEnd.RemoveAll(this);
		Weapon->OnWeaponActionStart.RemoveAll(this);
		Weapon->OnWeaponChamberCheck.RemoveAll(this);
		Weapon->OnWeaponHandsInHalf1.RemoveAll(this);
		Weapon->OnWeaponHandsInHalf2.RemoveAll(this);
		Weapon->OnWeaponHandsOutHalf1.RemoveAll(this);
		Weapon->OnWeaponHandsOutHalf2.RemoveAll(this);
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
		Weapon->OnWeaponReloadCatch.RemoveAll(this);
		Weapon->OnWeaponReloadCharge.RemoveAll(this);
		Weapon->OnWeaponRecoilGenerated.RemoveAll(this);
	}
}

UAnimInstance* UCharacterCombatComponent::GetRelevantAnimInstance() const
{
	return EquippedWeapon != nullptr ? EquippedWeapon->GetAnimInstance() : ShooterCharacter != nullptr ? ShooterCharacter->GetHandsAnimInstance() : nullptr;
}

void UCharacterCombatComponent::Server_EquipWeapon_Implementation(AWeapon* WeaponToEquip)
{
	if (CanEquipWeapon(WeaponToEquip))
	{
		NextWeapon = WeaponToEquip;
		SetCombatAction(ECombatAction::CA_IdleToOut);
	}
}

void UCharacterCombatComponent::Server_ReloadWeapon_Implementation()
{
	if (CanReloadWeapon())
	{
		SetCombatAction(ECombatAction::CA_MagOut);
	}
}

void UCharacterCombatComponent::Server_CheckWeaponChamber_Implementation()
{
	if (CanCheckWeaponChamber())
	{
		SetCombatAction(ECombatAction::CA_ChamberCheck);
	}
}

void UCharacterCombatComponent::Server_FireWeapon_Implementation(bool bFire)
{
	bWantsToFire = bFire;
	if (CanFireWeapon() && bFire)
	{
		Fire();
	}
}

void UCharacterCombatComponent::Server_ChangeWeaponFiremode_Implementation()
{
	if (CanChangeWeaponFiremode())
	{
		SetCombatAction(ECombatAction::CA_Firemode);
	}
}

void UCharacterCombatComponent::Server_CheckWeaponFiremode_Implementation()
{
	if (CanCheckWeaponFiremode())
	{
		SetCombatAction(ECombatAction::CA_FiremodeCheck);
	}
}

void UCharacterCombatComponent::Server_CheckWeaponMag_Implementation()
{
	if (CanCheckWeaponMag())
	{
		SetCombatAction(ECombatAction::CA_MagCheck);
	}
}

void UCharacterCombatComponent::Handle_OnCharacterHandsAnimInstanceIdle()
{
}

void UCharacterCombatComponent::Handle_OnCharacterHandsAnimInstanceIdleToOut()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		SetCombatAction(ECombatAction::CA_Out);
	}
}

void UCharacterCombatComponent::Handle_OnCharacterHandsAnimInstanceOut()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		SetEquippedWeapon(NextWeapon);
		NextWeapon = nullptr;
		
		OutToIdle();
	}
}

void UCharacterCombatComponent::Handle_OnCharacterHandsAnimInstanceOutToIdle()
{
	SetCombatAction(ECombatAction::CA_Idle);
}

void UCharacterCombatComponent::Handle_OnWeaponActionEnd(AWeapon* Weapon)
{
}

void UCharacterCombatComponent::Handle_OnWeaponActionStart(AWeapon* Weapon)
{
}

void UCharacterCombatComponent::Handle_OnWeaponChamberCheck(AWeapon* Weapon)
{
	SetCombatAction(ECombatAction::CA_Idle);
}

void UCharacterCombatComponent::Handle_OnWeaponHandsInHalf1(AWeapon* Weapon)
{
}

void UCharacterCombatComponent::Handle_OnWeaponHandsInHalf2(AWeapon* Weapon)
{
	SetCombatAction(ECombatAction::CA_Idle);
}

void UCharacterCombatComponent::Handle_OnWeaponHandsOutHalf1(AWeapon* Weapon)
{
}

void UCharacterCombatComponent::Handle_OnWeaponHandsOutHalf2(AWeapon* Weapon)
{
}

void UCharacterCombatComponent::Handle_OnWeaponFire(AWeapon* Weapon)
{
	//if (GetOwnerRole() == ROLE_Authority)
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
				Weapon->StopFireSound();

				SetCombatAction(ECombatAction::CA_Idle, true);
			}
			else if (Weapon->GetPatronInWeaponAmmo() == nullptr)
			{
				SetCombatAction(ECombatAction::CA_FireDry, true);
			}
		}
	}
}

void UCharacterCombatComponent::Handle_OnWeaponFireDry(AWeapon* Weapon)
{
	SetCombatAction(ECombatAction::CA_Idle);
}

void UCharacterCombatComponent::Handle_OnWeaponFiremode(AWeapon* Weapon)
{
	SetCombatAction(ECombatAction::CA_Idle);
}

void UCharacterCombatComponent::Handle_OnWeaponFiremodeCheck(AWeapon* Weapon)
{
	SetCombatAction(ECombatAction::CA_Idle);
}

void UCharacterCombatComponent::Handle_OnWeaponIdle(AWeapon* Weapon)
{
}

void UCharacterCombatComponent::Handle_OnWeaponIdleToOut(AWeapon* Weapon)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		SetCombatAction(ECombatAction::CA_Out);
	}
}

void UCharacterCombatComponent::Handle_OnWeaponMagCheck(AWeapon* Weapon)
{
	SetCombatAction(ECombatAction::CA_Idle);
}

void UCharacterCombatComponent::Handle_OnWeaponMagIn(AWeapon* Weapon)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		AMag* WeaponMag = Weapon != nullptr ? Weapon->GetMag() : nullptr;
		bool bHasMagAmmo = WeaponMag && WeaponMag->GetAmmoCount() > 0;
		bool bHasPatronInWeaponAmmo = Weapon && Weapon->GetPatronInWeaponAmmo() != nullptr;
		if (bHasMagAmmo && !bHasPatronInWeaponAmmo)
		{
			bool bIsBoltCatched = Weapon && Weapon->GetIsBoltCatched();
			SetCombatAction(bIsBoltCatched ? ECombatAction::CA_ReloadCatch : ECombatAction::CA_ReloadCharge);
		}
		else
		{
			SetCombatAction(ECombatAction::CA_HandsInHalf2);
		}
	}
}

void UCharacterCombatComponent::Handle_OnWeaponMagOut(AWeapon* Weapon)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		if (ShooterCharacter && ShooterCharacter->GetCharacterInventory())
		{
			ShooterCharacter->GetCharacterInventory()->LoadAmmoInWeaponMag(Weapon);
		}
		SetCombatAction(ECombatAction::CA_MagIn);
	}
}

void UCharacterCombatComponent::Handle_OnWeaponOut(AWeapon* Weapon)
{	
	if (GetOwnerRole() == ROLE_Authority)
	{
		SetEquippedWeapon(NextWeapon);
		NextWeapon = nullptr;
	
		OutToIdle();
	}
}

void UCharacterCombatComponent::Handle_OnWeaponOutToIdle(AWeapon* Weapon)
{
	SetCombatAction(ECombatAction::CA_Idle);
}

void UCharacterCombatComponent::Handle_OnWeaponOutToIdleArm(AWeapon* Weapon)
{
	SetCombatAction(ECombatAction::CA_Idle);
}

void UCharacterCombatComponent::Handle_OnWeaponReloadCatch(AWeapon* Weapon)
{
	SetCombatAction(ECombatAction::CA_Idle);
}

void UCharacterCombatComponent::Handle_OnWeaponReloadCharge(AWeapon* Weapon)
{
	SetCombatAction(ECombatAction::CA_Idle);
}

void UCharacterCombatComponent::Handle_OnWeaponRecoilGenerated(AWeapon* Weapon, float RecoilHorizontalKick, float RecoilVerticalKick)
{
	OnCharacterCombatWeaponRecoilGenerated.Broadcast(Weapon, RecoilHorizontalKick, RecoilVerticalKick);
}

void UCharacterCombatComponent::OnRep_CombatAction(ECombatAction PrevCombatAction)
{
	/*FText CombatActionText;
	UEnum::GetDisplayValueAsText(CombatAction, CombatActionText);*/

	ECombatAction AnimCombatAction = GetRelevantAnimCombatAction();

	if (UAnimInstance* AnimInstance = GetRelevantAnimInstance())
	{
		uint32 MachineIndex = AnimInstance->GetStateMachineIndex(TEXT("Default"));
		if (const FAnimNode_StateMachine* StateMachine = AnimInstance->GetStateMachineInstance(MachineIndex))
		{
			float AnimTimeRemaining = AnimInstance->GetRelevantAnimTimeRemaining(MachineIndex, StateMachine->GetCurrentState());
			//UE_LOG(LogTemp, Warning, TEXT("CombatAction: %s, CurrentStateName: %s: TimeRemaining: %f"), *CombatActionText.ToString(), *StateMachine->GetCurrentStateName().ToString(), AnimTimeRemaining);
			
			if ((CombatAction == ECombatAction::CA_Idle && AnimTimeRemaining > 0.0f) ||
				((CombatAction == ECombatAction::CA_Fire || CombatAction == ECombatAction::CA_FireDry) && AnimCombatAction == ECombatAction::CA_Idle))
			{
				CombatAction = AnimCombatAction;
			}
			else
			{
				OnCharacterCombatCombatActionChanged.Broadcast(CombatAction);
			}
		}
	}

}

void UCharacterCombatComponent::OnRep_EquippedWeapon(AWeapon* PrevEquippedWeapon)
{
	RemoveDelegates(PrevEquippedWeapon);
	AddDelegates(EquippedWeapon);

	OnCharacterCombatEquippedWeaponChanged.Broadcast(EquippedWeapon);
}
