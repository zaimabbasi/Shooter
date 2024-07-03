// Fill out your copyright notice in the Description page of Project Settings.


#include "ModComponent.h"
#include "Net/UnrealNetwork.h"
#include "Shooter/Data/ModDataAsset.h"
#include "Shooter/Mod/Barrel.h"
#include "Shooter/Mod/Charge.h"
//#include "Shooter/Mod/Flashlight.h"
//#include "Shooter/Mod/Foregrip.h"
#include "Shooter/Mod/GasBlock.h"
#include "Shooter/Mod/Handguard.h"
#include "Shooter/Mod/Mag.h"
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

	DOREPLIFETIME(UModComponent, Barrel);
	DOREPLIFETIME(UModComponent, Charge);
	DOREPLIFETIME(UModComponent, GasBlock);
	DOREPLIFETIME(UModComponent, Handguard);
	DOREPLIFETIME(UModComponent, Mag);
	DOREPLIFETIME(UModComponent, Muzzle);
	DOREPLIFETIME(UModComponent, Pistolgrip);
	DOREPLIFETIME(UModComponent, Reciever);
	DOREPLIFETIME(UModComponent, SightFront);
	DOREPLIFETIME(UModComponent, SightRear);
	DOREPLIFETIME(UModComponent, Stock);

}

void UModComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (OwningActor && OwningActor->HasAuthority() && ModDataAsset)
	{
		if (UWorld* World = GetWorld())
		{
			if (const TSubclassOf<ABarrel>& BarrelClass = ModDataAsset->BarrelClass)
			{
				if (ABarrel* SpawnedBarrel = World->SpawnActor<ABarrel>(BarrelClass))
				{
					SpawnedBarrel->SetOwner(OwningActor);
					if (SpawnedBarrel->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, TEXT("mod_barrel")))
					{
						Barrel = SpawnedBarrel;
					}
				}
			}
			if (const TSubclassOf<ACharge>& ChargeClass = ModDataAsset->ChargeClass)
			{
				if (ACharge* SpawnedCharge = World->SpawnActor<ACharge>(ChargeClass))
				{
					SpawnedCharge->SetOwner(OwningActor);
					if (SpawnedCharge->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, TEXT("mod_charge")))
					{
						Charge = SpawnedCharge;
					}
				}
			}
			if (const TSubclassOf<AGasBlock>& GasBlockClass = ModDataAsset->GasBlockClass)
			{
				if (AGasBlock* SpawnedGasBlock = World->SpawnActor<AGasBlock>(GasBlockClass))
				{
					SpawnedGasBlock->SetOwner(OwningActor);
					if (SpawnedGasBlock->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, TEXT("mod_gas_block")))
					{
						GasBlock = SpawnedGasBlock;
					}
				}
			}
			if (const TSubclassOf<AHandguard>& HandguardClass = ModDataAsset->HandguardClass)
			{
				if (AHandguard* SpawnedHandguard = World->SpawnActor<AHandguard>(HandguardClass))
				{
					SpawnedHandguard->SetOwner(OwningActor);
					if (SpawnedHandguard->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, TEXT("mod_handguard")))
					{
						Handguard = SpawnedHandguard;
					}
				}
			}
			if (const TSubclassOf<AMag>& MagClass = ModDataAsset->MagClass)
			{
				if (AMag* SpawnedMag = World->SpawnActor<AMag>(MagClass))
				{
					SpawnedMag->SetOwner(OwningActor);
					if (SpawnedMag->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, TEXT("mod_magazine")))
					{
						Mag = SpawnedMag;
					}
				}
			}
			if (const TSubclassOf<AMuzzle>& MuzzleClass = ModDataAsset->MuzzleClass)
			{
				if (AMuzzle* SpawnedMuzzle = World->SpawnActor<AMuzzle>(MuzzleClass))
				{
					SpawnedMuzzle->SetOwner(OwningActor);
					if (SpawnedMuzzle->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, TEXT("mod_muzzle")))
					{
						Muzzle = SpawnedMuzzle;
					}

				}
			}
			if (const TSubclassOf<APistolgrip>& PistolgripClass = ModDataAsset->PistolgripClass)
			{
				if (APistolgrip* SpawnedPistolgrip = World->SpawnActor<APistolgrip>(PistolgripClass))
				{
					SpawnedPistolgrip->SetOwner(OwningActor);
					if (SpawnedPistolgrip->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, TEXT("mod_pistol_grip")))
					{
						Pistolgrip = SpawnedPistolgrip;
					}
				}
			}
			if (const TSubclassOf<AReciever>& RecieverClass = ModDataAsset->RecieverClass)
			{
				if (AReciever* SpawnedReciever = World->SpawnActor<AReciever>(RecieverClass))
				{
					SpawnedReciever->SetOwner(OwningActor);
					if (SpawnedReciever->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, TEXT("mod_reciever")))
					{
						Reciever = SpawnedReciever;
					}
				}
			}
			if (const TSubclassOf<ASightFront>& SightFrontClass = ModDataAsset->SightFrontClass)
			{
				if (ASightFront* SpawnedSightFront = World->SpawnActor<ASightFront>(SightFrontClass))
				{
					SpawnedSightFront->SetOwner(OwningActor);
					if (SpawnedSightFront->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, TEXT("mod_sight_front")))
					{
						SightFront = SpawnedSightFront;
					}
				}
			}
			if (const TSubclassOf<ASightRear>& SightRearClass = ModDataAsset->SightRearClass)
			{
				if (ASightRear* SpawnedSightRear = World->SpawnActor<ASightRear>(SightRearClass))
				{
					SpawnedSightRear->SetOwner(OwningActor);
					if (SpawnedSightRear->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, TEXT("mod_sight_rear")))
					{
						SpawnedSightRear = SightRear;
					}
				}
			}
			if (const TSubclassOf<AStock>& StockClass = ModDataAsset->StockClass)
			{
				if (AStock* SpawnedStock = World->SpawnActor<AStock>(StockClass))
				{
					SpawnedStock->SetOwner(OwningActor);
					if (SpawnedStock->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, TEXT("mod_stock")))
					{
						Stock = SpawnedStock;
					}
				}
			}
		}
	}
}
