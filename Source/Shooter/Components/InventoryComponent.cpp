// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Shooter/Character/ShooterCharacter.h"
#include "Shooter/Data/CharacterDataAsset.h"
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

}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		if (OwningCharacter)
		{
			if (UCharacterDataAsset* CharacterDataAsset = OwningCharacter->GetCharacterDataAsset())
			{
				for (FInventoryData InventoryData : CharacterDataAsset->InventoryDataArray)
				{
					const TSubclassOf<AWeapon>& WeaponClass = InventoryData.WeaponClass;
					if (!WeaponClass)
					{
						continue;
					}
					if (UWorld* World = GetWorld())
					{
						AWeapon* NewWeapon = World->SpawnActor<AWeapon>(WeaponClass);
						NewWeapon->SetOwner(OwningCharacter);
						NewWeapon->SetActorHiddenInGame(true);
						WeaponsArray.Add(NewWeapon);
					}
				}
			}
		}
		if (IsLocallyControlled())
		{
			OnRepWeaponsArrayDelegate.Execute();
		}
	}
}

void UInventoryComponent::OnRep_WeaponsArray()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_WeaponsArray"));
	OnRepWeaponsArrayDelegate.Execute();
}

