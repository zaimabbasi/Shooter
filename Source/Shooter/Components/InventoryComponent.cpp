// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Shooter/Character/ShooterCharacter.h"
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
	DOREPLIFETIME(UInventoryComponent, CurrentIndex);

}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (OwningCharacter && OwningCharacter->HasAuthority())
	{
		for (const FInventoryData InventoryData : InventoryDataArray)
		{
			const TSubclassOf<AWeapon>& WeaponClass = InventoryData.WeaponClass;
			if (!WeaponClass)
			{
				continue;
			}
			if (UWorld* World = GetWorld())
			{
				if (AWeapon* SpawnedWeapon = World->SpawnActor<AWeapon>(WeaponClass))
				{
					SpawnedWeapon->SetOwner(OwningCharacter);
					SpawnedWeapon->SetActorHiddenInGameWithChildren(true);
					WeaponsArray.Add(SpawnedWeapon);
				}
			}
		}

		if (OwningCharacter->IsLocallyControlled())
		{
			OnWeaponsArrayReadyDelegate.Execute();
		}
	}

}

void UInventoryComponent::OnRep_WeaponsArray() const
{
	OnWeaponsArrayReadyDelegate.Execute();
}

void UInventoryComponent::Server_SetCurrentIndex_Implementation(uint8 Index)
{
	if (!WeaponsArray.IsValidIndex(Index))
	{
		return;
	}
	CurrentIndex = Index;
}

AWeapon* UInventoryComponent::GetWeaponAtIndex(uint32 Index)
{
	if (WeaponsArray.IsValidIndex(Index))
	{
		return WeaponsArray[Index];
	}
	return nullptr;
}
