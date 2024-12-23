// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/ModComponent.h"
#include "Net/UnrealNetwork.h"
#include "Actor/Mag.h"
#include "Actor/Mod.h"
#include "Actor/Weapon.h"
#include "DataAsset/ModDataAsset.h"
#include "Struct/ShooterUtility.h"
#include "Type/ShooterNameType.h"

UModComponent::UModComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UModComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UModComponent, ModArray, COND_OwnerOnly);
	DOREPLIFETIME(UModComponent, Mag);

}

void UModComponent::Init(const UModDataAsset* ModDataAsset)
{
	if (ModDataAsset == nullptr)
	{
		return;
	}
	if (AActor* OwningActor = GetOwner())
	{
		if (UWorld* World = GetWorld())
		{
			for (const TSubclassOf<AMod>& ModClass : ModDataAsset->ModClassArray)
			{
				if (AMod* SpawnedMod = World->SpawnActor<AMod>(ModClass))
				{
					SpawnedMod->SetOwner(OwningActor);
					SpawnedMod->Init();
					SpawnedMod->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, SpawnedMod->GetDefaultAttachParentSocketName());

					ModArray.Add(SpawnedMod);

					if (SpawnedMod->IsA(AMag::StaticClass()))
					{
						Mag = Cast<AMag>(SpawnedMod);
					}
				}
			}
		}
	}
}

void UModComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UModComponent::BeginPlay()
{
	Super::BeginPlay();

}
