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
				//if (const TSubclassOf<AMod>& ModClass = ModData.ModClass)
				{
					if (AMod* SpawnedMod = World->SpawnActor<AMod>(ModData.ModClass))
					{
						SpawnedMod->SetOwner(OwningActor);
						SpawnedMod->Init();
						SpawnedMod->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, ModData.AttachParentSocketName);

						ModArray.Add(SpawnedMod);
					}
				}
			}
			
		}
	}
}
