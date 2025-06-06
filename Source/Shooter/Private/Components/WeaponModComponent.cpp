// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/WeaponModComponent.h"
#include "Net/UnrealNetwork.h"
#include "Data/WeaponModDataAsset.h"
#include "Mod/Mod.h"
#include "Types/ShooterNames.h"
#include "Utility/ShooterUtility.h"

UWeaponModComponent::UWeaponModComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UWeaponModComponent::BeginPlay()
{
	Super::BeginPlay();

}

AMod* UWeaponModComponent::SpawnMod(TSubclassOf<AMod> ModClass, AActor* ParentActor, FName AttachParentSocketName)
{
	if (UWorld* World = GetWorld())
	{
		if (AMod* SpawnedMod = World->SpawnActor<AMod>(ModClass))
		{
			SpawnedMod->SetOwner(GetOwner());
			SpawnedMod->Init();
			SpawnedMod->AttachToActor(ParentActor, FAttachmentTransformRules::KeepRelativeTransform, AttachParentSocketName);
			
			ModArray.Add(SpawnedMod);

			return SpawnedMod;
		}
	}
	return nullptr;
}

//void UWeaponModComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//}

void UWeaponModComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UWeaponModComponent, ModArray, COND_OwnerOnly);

}

void UWeaponModComponent::Init(const UWeaponModDataAsset* WeaponModDataAsset)
{
	if (WeaponModDataAsset == nullptr)
	{
		return;
	}
	for (const FModData& ModData : WeaponModDataAsset->ModDataArray)
	{
		if (ModData.ModParentClass)
		{
			// Check parent is in the ModDataArray?
			const FModData* ModParentData = WeaponModDataAsset->ModDataArray.FindByPredicate([&](const FModData& ModDataInArray) {
				return ModDataInArray.ModClass == ModData.ModParentClass;
				});
			if (ModParentData)
			{
				// Check already exists?
				TObjectPtr<AMod>* Mod = ModArray.FindByPredicate([&](AMod* const& Mod) {
					return Mod->IsA(ModParentData->ModClass);
					});
				if (Mod)
				{
					SpawnMod(ModData.ModClass, *Mod, ModData.AttachParentSocketName);
				}
				else
				{
					AMod* ParentMod = SpawnMod(ModParentData->ModClass, GetOwner(), ModParentData->AttachParentSocketName);
					SpawnMod(ModData.ModClass, ParentMod, ModData.AttachParentSocketName);
				}
			}
			else
			{
				// Mod parent is not valid, so do not spawn mod
			}
		}
		else
		{
			// Check already exists?
			const FModData* ModParentData = WeaponModDataAsset->ModDataArray.FindByPredicate([&](const FModData& ModDataInArray) {
				return ModDataInArray.ModClass == ModData.ModParentClass;
				});
			if (!ModParentData)
			{
				SpawnMod(ModData.ModClass, GetOwner(), ModData.AttachParentSocketName);
			}
		}
	}
}
