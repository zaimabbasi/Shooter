// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Actor/Mag.h"
#include "Actor/Weapon.h"
#include "DataAsset/CharacterDataAsset.h"
#include "Character/ShooterCharacter.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UInventoryComponent, WeaponArray, COND_OwnerOnly);
	DOREPLIFETIME(UInventoryComponent, WeaponAmmoArray);

}

void UInventoryComponent::Init(const FInventoryParams& InventoryParams)
{
	if (UWorld* World = GetWorld())
	{
		AActor* OwningActor = GetOwner();
		if (AWeapon* const PrimaryWeapon  = World->SpawnActor<AWeapon>(InventoryParams.PrimaryWeaponClass))
		{
			PrimaryWeapon->SetOwner(OwningActor);
			PrimaryWeapon->Init();
			WeaponArray.Add(PrimaryWeapon);
		}
		if (AWeapon* const SecondaryWeapon = World->SpawnActor<AWeapon>(InventoryParams.SecondaryWeaponClass))
		{
			SecondaryWeapon->SetOwner(OwningActor);
			SecondaryWeapon->Init();
			WeaponArray.Add(SecondaryWeapon);
		}
	}
	WeaponAmmoArray.Add(InventoryParams.PrimaryWeaponMaxAmmo);
	WeaponAmmoArray.Add(InventoryParams.SecondaryWeaponMaxAmmo);

}

int8 UInventoryComponent::FindWeapon(AWeapon* &Weapon) const
{
	return WeaponArray.Find(Weapon);
}

void UInventoryComponent::LoadAmmoInWeaponMag(AWeapon* Weapon, const uint8 AmmoCount)
{
	if (Weapon == nullptr || AmmoCount == 0)
	{
		return;
	}
	Server_LoadAmmoInWeaponMag(Weapon, AmmoCount);
}

void UInventoryComponent::Server_LoadAmmoInWeaponMag_Implementation(AWeapon* Weapon, const uint8 AmmoCount)
{
	if (Weapon == nullptr || AmmoCount == 0)
	{
		return;
	}
	const int8 WeaponIndex = FindWeapon(Weapon);
	if (WeaponAmmoArray.IsValidIndex(WeaponIndex))
	{
		const uint8 WeaponAmmo = WeaponAmmoArray[WeaponIndex];
		if (WeaponAmmo >= AmmoCount)
		{
			if (AMag* WeaponMag = Weapon->GetMag())
			{
				WeaponMag->AddAmmo(AmmoCount);
				WeaponAmmoArray[WeaponIndex] -= AmmoCount;
			}
		}
	}
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

}

AWeapon* UInventoryComponent::GetWeaponAtIndex(uint8 Index)
{
	return WeaponArray.IsValidIndex(Index) ? WeaponArray[Index] : nullptr;
}

uint8 UInventoryComponent::GetAmmoAtIndex(uint8 Index)
{
	return WeaponAmmoArray.IsValidIndex(Index) ? WeaponAmmoArray[Index] : 0;
}

void UInventoryComponent::OnRep_WeaponArray() const
{
	OnInventoryComponentWeaponArrayReplicated.Broadcast();
}
