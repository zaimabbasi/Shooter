// Fill out your copyright notice in the Description page of Project Settings.


#include "ModComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "Shooter/Mod/Barrel.h"
#include "Shooter/Mod/Charge.h"
//#include "Shooter/Mod/Flashlight.h"
//#include "Shooter/Mod/Foregrip.h"
#include "Shooter/Mod/GasBlock.h"
#include "Shooter/Mod/Handguard.h"
#include "Shooter/Mod/Mag.h"
#include "Shooter/Mod/Mod.h"
//#include "Shooter/Mod/Mount.h"
#include "Shooter/Mod/Muzzle.h"
#include "Shooter/Mod/Reciever.h"
#include "Shooter/Mod/Pistolgrip.h"
//#include "Shooter/Mod/Scope.h"
#include "Shooter/Mod/SightFront.h"
#include "Shooter/Mod/SightRear.h"
#include "Shooter/Mod/Stock.h"
//#include "Shooter/Mod/Tactical.h"
#include "Shooter/Weapon/Weapon.h"
#include "Shooter/Utility/ShooterUtility.h"

UModComponent::UModComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UModComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UModComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UModComponent, ModArray);

}

void UModComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (OwningActor && OwningActor->HasAuthority())
	{
		if (UWorld* World = GetWorld())
		{
			for (const FModData ModData : ModDataArray)
			{
				if (const TSubclassOf<AMod>& ModClass = ModData.ModClass)
				{
					if (AMod* SpawnedMod = World->SpawnActor<AMod>(ModClass))
					{
						SpawnedMod->SetOwner(OwningActor);
						SpawnedMod->SetModType(ModData.ModType);
						if (OwningActorMesh)
						{
							FName AttachmentSocketName = FShooterUtility::GetModAttachmentSocketName(ModData.ModType);
							if (OwningActorMesh->DoesSocketExist(AttachmentSocketName))
							{
								if (const USkeletalMeshSocket* ModSocket = OwningActorMesh->GetSocketByName(AttachmentSocketName))
								{
									ModSocket->AttachActor(SpawnedMod, OwningActorMesh);
								}
							}
						}
						ModArray.Add(SpawnedMod);
					}	
				}
			}
		}
	}
}
