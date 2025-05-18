// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/WeaponModComponent.h"
#include "Net/UnrealNetwork.h"
#include "Data/ModDataAsset.h"
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
			AActor* OwningActor = GetOwner();
			SpawnedMod->SetOwner(OwningActor);
			//SpawnedMod->Init();
			SpawnedMod->AttachToActor(ParentActor, FAttachmentTransformRules::KeepRelativeTransform, AttachParentSocketName);
			
			ModArray.Add(SpawnedMod);
		}
		return nullptr;
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

//void UWeaponModComponent::Init(const UModDataAsset* ModDataAsset)
//{
//	if (ModDataAsset == nullptr)
//	{
//		return;
//	}
//	if (AActor* OwningActor = GetOwner())
//	{
//		if (UWorld* World = GetWorld())
//		{
//			for (const FModData& ModData : ModDataAsset->ModDataArray)
//			{
//				//if (const TSubclassOf<AMod>& ModClass = ModData.ModClass)
//				{
//					if (AMod* SpawnedMod = World->SpawnActor<AMod>(ModData.ModClass))
//					{
//						SpawnedMod->SetOwner(OwningActor);
//						//SpawnedMod->Init();
//						SpawnedMod->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, ModData.AttachParentSocketName);
//
//						ModArray.Add(SpawnedMod);
//					}
//				}
//			}
//			
//		}
//	}
//}

void UWeaponModComponent::Init(const UModDataAsset* ModDataAsset)
{
	if (ModDataAsset == nullptr)
	{
		return;
	}
	if (AActor* OwningActor = GetOwner())
	{
		if (UWorld* World = GetWorld())
		{
			for (const FModData& ModData : ModDataAsset->ModDataArray)
			{
				if (ModData.ModParentClass)
				{
					// Check parent is in the ModDataArray?
					const FModData* ModParentData = ModDataAsset->ModDataArray.FindByPredicate([&](const FModData& ModDataInArray) {
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
					const FModData* ModParentData = ModDataAsset->ModDataArray.FindByPredicate([&](const FModData& ModDataInArray) {
						return ModDataInArray.ModClass == ModData.ModParentClass;
						});
					if (!ModParentData)
					{
						SpawnMod(ModData.ModClass, GetOwner(), ModData.AttachParentSocketName);
					}
				}
			}
		}
	}
}
