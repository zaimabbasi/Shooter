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

	if (HasAuthority())
	{
		for (FInventoryData InventoryData : InventoryDataArray)
		{
			const TSubclassOf<AWeapon>& WeaponClass = InventoryData.WeaponClass;
			if (!WeaponClass)
			{
				continue;
			}
			if (UWorld* World = GetWorld())
			{
				if (AWeapon* NewWeapon = World->SpawnActor<AWeapon>(WeaponClass))
				{
					NewWeapon->SetOwner(OwningCharacter);
					NewWeapon->SetActorHiddenInGame(true);
					WeaponsArray.Add(NewWeapon);
				}
				if (AWeapon* NewWeapon1P = World->SpawnActor<AWeapon>(WeaponClass))
				{
					NewWeapon1P->SetOwner(OwningCharacter);
					NewWeapon1P->SetActorHiddenInGame(true);
					if (IsLocallyControlled())
					{
						NewWeapon1P->SetReplicates(false);
					}
					if (USkeletalMeshComponent* WeaponMesh = NewWeapon1P->GetMesh())
					{
						WeaponMesh->SetCastShadow(false);
					}
					WeaponsArray1P.Add(NewWeapon1P);
				}
			}
		}
		if (IsLocallyControlled())
		{
			OnRepWeaponsArrayDelegate.Execute();
			OnRepWeaponsArrayDelegate1P.Execute();
		}
	}
	
}

void UInventoryComponent::OnRep_WeaponsArray()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_WeaponsArray"));
	
	OnRepWeaponsArrayDelegate.Execute();
}

void UInventoryComponent::OnRep_WeaponsArray1P()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_WeaponsArray1P"));

	for (int32 i = 0; i < WeaponsArray1P.Num(); ++i)
	{
		if (AWeapon* Weapon1P = WeaponsArray1P[i])
		{
			if (USkeletalMeshComponent* WeaponMesh = Weapon1P->GetMesh())
			{
				WeaponMesh->SetCastShadow(false);
			}
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
