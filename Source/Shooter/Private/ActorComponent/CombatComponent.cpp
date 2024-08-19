// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/CombatComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "Actor/Weapon.h"
#include "Character/ShooterCharacter.h"
#include "Struct/ShooterUtility.h"

UCombatComponent::UCombatComponent() :
	EquippedWeapon(nullptr)
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
			EquippedWeapon->ClearAnimInstanceDelegateBindings();
			EquippedWeapon->ClearAnimInstance();
		}
		if (WeaponToEquip)
		{
			WeaponToEquip->WeaponAction = EWeaponAction::WAS_OutToIdle;
			WeaponToEquip->SetAnimInstance();
			WeaponToEquip->SetAnimInstanceDelegateBindings();
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
		EquippedWeapon->ClearAnimInstanceDelegateBindings();
		EquippedWeapon->ClearAnimInstance();
	}
	if (WeaponToEquip)
	{
		WeaponToEquip->WeaponAction = EWeaponAction::WAS_OutToIdle;
		WeaponToEquip->SetAnimInstance();
		WeaponToEquip->SetAnimInstanceDelegateBindings();
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
		FShooterUtility::AttachActor(EquippedWeapon, ParentSkeletalMesh, ParentSocketName);
	}
	Server_WeaponAttach(ParentSkeletalMesh, ParentSocketName);
}

void UCombatComponent::Server_WeaponAttach_Implementation(USkeletalMeshComponent* ParentSkeletalMesh, FName ParentSocketName)
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	FShooterUtility::AttachActor(EquippedWeapon, ParentSkeletalMesh, ParentSocketName);
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

void UCombatComponent::WeaponIdleToOut()
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	EquippedWeapon->SetWeaponAction(EWeaponAction::WAS_IdleToOut);
}

void UCombatComponent::WeaponOutToIdle()
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	EquippedWeapon->SetWeaponAction(EWeaponAction::WAS_OutToIdle);
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

void UCombatComponent::Handle_OnWeaponIdle(AWeapon* Weapon)
{
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));
}

void UCombatComponent::Handle_OnWeaponIdleToOut(AWeapon* Weapon)
{
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));
	if (Weapon)
	{
		Weapon->WeaponAction = EWeaponAction::WAS_Out;
	}
}

void UCombatComponent::Handle_OnWeaponOut(AWeapon* Weapon)
{
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));

	OnCombatComponentWeaponOut.Broadcast(Weapon);
}

void UCombatComponent::Handle_OnWeaponOutToIdle(AWeapon* Weapon)
{
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));
	if (Weapon)
	{
		Weapon->WeaponAction = EWeaponAction::WAS_Idle;
	}
}

void UCombatComponent::OnRep_EquippedWeapon(AWeapon* PrevEquippedWeapon)
{
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));
	if (PrevEquippedWeapon)
	{
		PrevEquippedWeapon->ClearAnimInstance();
	}
	if (EquippedWeapon)
	{
		EquippedWeapon->WeaponAction = EWeaponAction::WAS_OutToIdle;
		EquippedWeapon->SetAnimInstance();
	}
	
	OnCombatComponentEquippedWeaponReplicated.Broadcast(EquippedWeapon, PrevEquippedWeapon);
}

void UCombatComponent::Server_SetIsAiming_Implementation(bool bAiming)
{
	if (EquippedWeapon == nullptr)
	{
		return;
	}
	bIsAiming = bAiming;
}
