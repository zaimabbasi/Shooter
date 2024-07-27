// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Actor/Mag.h"
#include "Actor/Weapon.h"
#include "Character/ShooterCharacter.h"
#include "DataAsset/InventoryDataAsset.h"

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

	DOREPLIFETIME(UInventoryComponent, WeaponArray);
	DOREPLIFETIME(UInventoryComponent, WeaponAmmoArray);

}

void UInventoryComponent::Init(const UInventoryDataAsset* InventoryDataAsset)
{
	if (InventoryDataAsset == nullptr)
	{
		return;
	}
	if (UWorld* World = GetWorld())
	{
		AActor* OwningActor = GetOwner();
		if (AWeapon* PrimaryWeapon = World->SpawnActor<AWeapon>(InventoryDataAsset->PrimaryWeaponClass))
		{
			PrimaryWeapon->SetOwner(OwningActor);
			PrimaryWeapon->Init();
			WeaponArray.Add(PrimaryWeapon);
		}
		if (AWeapon* SecondaryWeapon = World->SpawnActor<AWeapon>(InventoryDataAsset->SecondaryWeaponClass))
		{
			SecondaryWeapon->SetOwner(OwningActor);
			SecondaryWeapon->Init();
			WeaponArray.Add(SecondaryWeapon);
		}
	}
	WeaponAmmoArray.Add(InventoryDataAsset->PrimaryWeaponMaxAmmo);
	WeaponAmmoArray.Add(InventoryDataAsset->SecondaryWeaponMaxAmmo);

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

//void UInventoryComponent::Server_AddAmmoInWeaponMag_Implementation(uint8 AmmoCount, uint8 WeaponIndex)
//{
//	if (AmmoCount == 0)
//	{
//		return;
//	}
//	if (AWeapon* Weapon = GetWeaponAtIndex(WeaponIndex))
//	{
//		if (AMag* WeaponMag = Weapon->GetMag())
//		{
//			uint8 WeaponAmmo = GetAmmoAtIndex(WeaponIndex);
//			uint8 WeaponMagAmmoSpace = WeaponMag->GetAmmoSpace();
//			if (WeaponAmmo > 0 && WeaponMagAmmoSpace > 0 && AmmoCount <= WeaponMagAmmoSpace && AmmoCount <= WeaponAmmo)
//			{
//				WeaponMag->AddAmmo(AmmoCount);
//				WeaponAmmoArray[WeaponIndex] -= AmmoCount;
//			}
//		}
//	}
//}

AWeapon* UInventoryComponent::GetWeaponAtIndex(uint8 Index)
{
	return WeaponArray.IsValidIndex(Index) ? WeaponArray[Index] : nullptr;
}

uint8 UInventoryComponent::GetAmmoAtIndex(uint8 Index)
{
	return WeaponAmmoArray.IsValidIndex(Index) ? WeaponAmmoArray[Index] : 0;
}
