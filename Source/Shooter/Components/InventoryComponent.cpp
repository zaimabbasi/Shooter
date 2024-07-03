// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Shooter/Character/ShooterCharacter.h"
#include "Shooter/Data/InventoryDataAsset.h"
#include "Shooter/Mod/Mag.h"
#include "Shooter/Weapon/Weapon.h"

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

	DOREPLIFETIME_CONDITION(UInventoryComponent, WeaponsArray, COND_OwnerOnly);
	DOREPLIFETIME(UInventoryComponent, WeaponsAmmoArray);

}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (OwningCharacter && OwningCharacter->HasAuthority())
	{
		if (InventoryDataAsset)
		{
			if (UWorld* World = GetWorld())
			{
				if (const TSubclassOf<AWeapon>& PrimaryWeaponClass = InventoryDataAsset->PrimaryWeaponClass)
				{
					if (AWeapon* SpawnedPrimaryWeapon = World->SpawnActor<AWeapon>(PrimaryWeaponClass))
					{
						SpawnedPrimaryWeapon->SetOwner(OwningCharacter);
						WeaponsArray.Add(SpawnedPrimaryWeapon);
					}
				}
				if (const TSubclassOf<AWeapon>& SecondaryWeaponClass = InventoryDataAsset->SecondaryWeaponClass)
				{
					if (AWeapon* SpawnedSecondaryWeapon = World->SpawnActor<AWeapon>(SecondaryWeaponClass))
					{
						SpawnedSecondaryWeapon->SetOwner(OwningCharacter);
						WeaponsArray.Add(SpawnedSecondaryWeapon);
					}
				}
			}

			WeaponsAmmoArray.Add(InventoryDataAsset->PrimaryWeaponAmmo);
			WeaponsAmmoArray.Add(InventoryDataAsset->SecondaryWeaponAmmo);
		}

		for (uint8 Index = 0; Index < WeaponsArray.Num(); ++Index)
		{
			if (AWeapon* Weapon = GetWeaponAtIndex(Index))
			{
				if (AMag* WeaponMag = Weapon->GetMag())
				{
					Server_AddAmmoInWeaponMag(WeaponMag->GetAmmoSpace(), Index);
				}
			}
		}
		
		if (OwningCharacter->IsLocallyControlled())
		{
			OnWeaponsArrayReadyDelegate.Execute();
		}
	}

}

void UInventoryComponent::Server_AddAmmoInWeaponMag_Implementation(uint8 AmmoCount, uint8 WeaponIndex)
{
	if (AmmoCount == 0)
	{
		return;
	}
	if (AWeapon* Weapon = GetWeaponAtIndex(WeaponIndex))
	{
		if (AMag* WeaponMag = Weapon->GetMag())
		{
			uint8 WeaponAmmo = GetAmmoAtIndex(WeaponIndex);
			uint8 WeaponMagAmmoSpace = WeaponMag->GetAmmoSpace();
			if (WeaponAmmo > 0 && WeaponMagAmmoSpace > 0 && AmmoCount <= WeaponMagAmmoSpace && AmmoCount <= WeaponAmmo)
			{
				WeaponMag->AddAmmo(AmmoCount);
				WeaponsAmmoArray[WeaponIndex] -= AmmoCount;
			}
		}
	}
}

void UInventoryComponent::OnRep_WeaponsArray() const
{
	OnWeaponsArrayReadyDelegate.Execute();
}

AWeapon* UInventoryComponent::GetWeaponAtIndex(uint8 Index)
{
	return WeaponsArray.IsValidIndex(Index) ? WeaponsArray[Index] : nullptr;
}

uint8 UInventoryComponent::GetAmmoAtIndex(uint8 Index)
{
	return WeaponsAmmoArray.IsValidIndex(Index) ? WeaponsAmmoArray[Index] : 0;
}
