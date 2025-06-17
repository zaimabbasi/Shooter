// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CharacterInventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Data/CharacterDataAsset.h"
#include "Mod/Mag.h"
#include "Weapon/Weapon.h"
#include "Character/ShooterCharacter.h"

UCharacterInventoryComponent::UCharacterInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UCharacterInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

}

//void UCharacterInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//}

void UCharacterInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UCharacterInventoryComponent, WeaponArray, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UCharacterInventoryComponent, WeaponAmmoArray, COND_OwnerOnly);

}

void UCharacterInventoryComponent::Init(const FInventoryParams& InventoryParams)
{
	if (UWorld* World = GetWorld())
	{
		AActor* OwningActor = GetOwner();
		if (AWeapon* const PrimaryWeapon = World->SpawnActor<AWeapon>(InventoryParams.PrimaryWeaponClass))
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

int8 UCharacterInventoryComponent::FindWeapon(AWeapon* const& Weapon) const
{
	return WeaponArray.Find(Weapon);
}

uint8 UCharacterInventoryComponent::GetWeaponAmmoAtIndex(uint8 Index) const
{
	return WeaponAmmoArray.IsValidIndex(Index) ? WeaponAmmoArray[Index] : 0;
}

uint8 UCharacterInventoryComponent::GetWeaponAmmo(AWeapon* Weapon) const
{
	int8 WeaponIndex = FindWeapon(Weapon);
	return WeaponIndex != INDEX_NONE ? GetWeaponAmmoAtIndex(WeaponIndex) : 0;
}

void UCharacterInventoryComponent::LoadAmmoInWeaponMag(AWeapon* Weapon)
{
	int8 WeaponIndex = FindWeapon(Weapon);
	if (WeaponIndex != INDEX_NONE)
	{
		if (AMag* WeaponMag = Weapon->GetMag())
		{
			uint8 AmmoCount = FMath::Min(WeaponMag->GetAmmoSpace(), GetWeaponAmmoAtIndex(WeaponIndex));
			if (AmmoCount > 0)
			{
				WeaponMag->AddAmmo(AmmoCount);
				WeaponAmmoArray[WeaponIndex] -= AmmoCount;
			}
		}
	}
}

AWeapon* UCharacterInventoryComponent::GetWeaponAtIndex(uint8 Index) const
{
	return WeaponArray.IsValidIndex(Index) ? WeaponArray[Index] : nullptr;
}

void UCharacterInventoryComponent::OnRep_WeaponArray() const
{
	OnCharacterInventoryWeaponArrayReplicated.Broadcast();
}
