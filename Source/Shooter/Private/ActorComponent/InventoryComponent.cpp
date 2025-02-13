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

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

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

int8 UInventoryComponent::FindWeapon(AWeapon*& Weapon) const
{
	return WeaponArray.Find(Weapon);
}

uint8 UInventoryComponent::GetWeaponAmmoAtIndex(uint8 Index) const
{
	return WeaponAmmoArray.IsValidIndex(Index) ? WeaponAmmoArray[Index] : 0;
}

void UInventoryComponent::LoadAmmoInWeaponMag(uint8 WeaponIndex)
{
	if (const AWeapon* Weapon = GetWeaponAtIndex(WeaponIndex))
	{
		if (AMag* WeaponMag = Weapon->GetMag())
		{
			const uint8 MagAmmoSpace = WeaponMag->GetAmmoCapacity() - WeaponMag->GetAmmoCount();
			const uint8 AmmoCount = FMath::Min(MagAmmoSpace, GetWeaponAmmoAtIndex(WeaponIndex));
			if (AmmoCount > 0)
			{
				WeaponMag->AddAmmo(AmmoCount);
				WeaponAmmoArray[WeaponIndex] -= AmmoCount;
			}
		}
	}
}

AWeapon* UInventoryComponent::GetWeaponAtIndex(uint8 Index) const
{
	return WeaponArray.IsValidIndex(Index) ? WeaponArray[Index] : nullptr;
}

void UInventoryComponent::OnRep_WeaponArray() const
{
	OnInventoryComponentWeaponArrayReplicated.Broadcast();
}
