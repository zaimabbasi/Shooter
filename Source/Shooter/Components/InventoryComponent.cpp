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
	DOREPLIFETIME_CONDITION(UInventoryComponent, WeaponsArray1P, COND_OwnerOnly);

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
					SpawnedWeapon->SetActorHiddenInGame(true);
					if (OwningCharacter->IsLocallyControlled())
					{
						if (USkeletalMeshComponent* WeaponMesh = SpawnedWeapon->GetMesh())
						{
							WeaponMesh->SetVisibility(false);
							WeaponMesh->SetCastHiddenShadow(true);
						}
					}
					WeaponsArray.Add(SpawnedWeapon);
				}
				if (AWeapon* SpawnedWeapon1P = World->SpawnActor<AWeapon>(WeaponClass))
				{
					SpawnedWeapon1P->SetOwner(OwningCharacter);
					SpawnedWeapon1P->SetActorHiddenInGame(true);
					if (OwningCharacter->IsLocallyControlled())
					{
						SpawnedWeapon1P->SetReplicates(false);
					}
					if (USkeletalMeshComponent* WeaponMesh = SpawnedWeapon1P->GetMesh())
					{
						WeaponMesh->SetCastShadow(false);
					}
					WeaponsArray1P.Add(SpawnedWeapon1P);
				}
			}
		}
		if (OwningCharacter->IsLocallyControlled())
		{
			OnRepWeaponsArrayDelegate.Execute();
			OnRepWeaponsArrayDelegate1P.Execute();
		}
	}
	
}

void UInventoryComponent::OnRep_WeaponsArray()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_WeaponsArray: %i"), WeaponsArray.Num());

	for (const AWeapon* Weapon : WeaponsArray)
	{
		if (Weapon == nullptr)
		{
			continue;
		}
		if (USkeletalMeshComponent* WeaponMesh = Weapon->GetMesh())
		{
			WeaponMesh->SetVisibility(false);
			WeaponMesh->SetCastHiddenShadow(true);
		}
	}
	OnRepWeaponsArrayDelegate.Execute();
}

void UInventoryComponent::OnRep_WeaponsArray1P()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_WeaponsArray1P: %i"), WeaponsArray1P.Num());

	for (const AWeapon* Weapon1P : WeaponsArray1P)
	{
		if (Weapon1P == nullptr)
		{
			continue;
		}
		if (USkeletalMeshComponent* WeaponMesh = Weapon1P->GetMesh())
		{
			WeaponMesh->SetCastShadow(false);
		}
	}
	OnRepWeaponsArrayDelegate1P.Execute();
}

AWeapon* UInventoryComponent::GetWeaponAtIndex(uint32 index)
{
	if (WeaponsArray.IsValidIndex(index))
	{
		return WeaponsArray[index];
	}
	
	return nullptr;
}

AWeapon* UInventoryComponent::GetWeaponAtIndex1P(uint32 index)
{
	if (WeaponsArray1P.IsValidIndex(index))
	{
		return WeaponsArray1P[index];
	}

	return nullptr;
}
