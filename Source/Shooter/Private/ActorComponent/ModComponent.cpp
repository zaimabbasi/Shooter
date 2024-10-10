// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/ModComponent.h"
#include "Net/UnrealNetwork.h"
#include "Actor/Barrel.h"
#include "Actor/Charge.h"
//#include "Actor/Flashlight.h"
//#include "Actor/Foregrip.h"
#include "Actor/GasBlock.h"
#include "Actor/Handguard.h"
#include "Actor/Mag.h"
//#include "Actor/Mount.h"
#include "Actor/Muzzle.h"
#include "Actor/Reciever.h"
#include "Actor/Pistolgrip.h"
//#include "Actor/Scope.h"
#include "Actor/SightFront.h"
#include "Actor/SightRear.h"
#include "Actor/Stock.h"
//#include "Actor/Tactical.h"
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
			if (const TSubclassOf<ABarrel>& BarrelClass = ModDataAsset->BarrelClass)
			{
				if (ABarrel* SpawnedBarrel = World->SpawnActor<ABarrel>(BarrelClass))
				{
					SpawnedBarrel->SetOwner(OwningActor);
					SpawnedBarrel->Init();
					SpawnedBarrel->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, MOD_BARREL_SOCKET_NAME);

					Barrel = SpawnedBarrel;
				}
			}
			if (const TSubclassOf<ACharge>& ChargeClass = ModDataAsset->ChargeClass)
			{
				if (ACharge* SpawnedCharge = World->SpawnActor<ACharge>(ChargeClass))
				{
					SpawnedCharge->SetOwner(OwningActor);
					SpawnedCharge->Init();
					SpawnedCharge->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, MOD_CHARGE_SOCKET_NAME);

					Charge = SpawnedCharge;
				}
			}
			if (const TSubclassOf<AGasBlock>& GasBlockClass = ModDataAsset->GasBlockClass)
			{
				if (AGasBlock* SpawnedGasBlock = World->SpawnActor<AGasBlock>(GasBlockClass))
				{
					SpawnedGasBlock->SetOwner(OwningActor);
					SpawnedGasBlock->Init();
					SpawnedGasBlock->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, MOD_GAS_BLOCK_SOCKET_NAME);

					GasBlock = SpawnedGasBlock;
				}
			}
			if (const TSubclassOf<AHandguard>& HandguardClass = ModDataAsset->HandguardClass)
			{
				if (AHandguard* SpawnedHandguard = World->SpawnActor<AHandguard>(HandguardClass))
				{
					SpawnedHandguard->SetOwner(OwningActor);
					SpawnedHandguard->Init();
					SpawnedHandguard->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, MOD_HANDGUARD_SOCKET_NAME);

					Handguard = SpawnedHandguard;
				}
			}
			if (const TSubclassOf<AMag>& MagClass = ModDataAsset->MagClass)
			{
				if (AMag* SpawnedMag = World->SpawnActor<AMag>(MagClass))
				{
					SpawnedMag->SetOwner(OwningActor);
					SpawnedMag->Init();
					SpawnedMag->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, MOD_MAGAZINE_SOCKET_NAME);

					Mag = SpawnedMag;
				}
			}
			if (const TSubclassOf<AMuzzle>& MuzzleClass = ModDataAsset->MuzzleClass)
			{
				if (AMuzzle* SpawnedMuzzle = World->SpawnActor<AMuzzle>(MuzzleClass))
				{
					SpawnedMuzzle->SetOwner(OwningActor);
					SpawnedMuzzle->Init();
					SpawnedMuzzle->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, MOD_MUZZLE_SOCKET_NAME);

					Muzzle = SpawnedMuzzle;
				}
			}
			if (const TSubclassOf<APistolgrip>& PistolgripClass = ModDataAsset->PistolgripClass)
			{
				if (APistolgrip* SpawnedPistolgrip = World->SpawnActor<APistolgrip>(PistolgripClass))
				{
					SpawnedPistolgrip->SetOwner(OwningActor);
					SpawnedPistolgrip->Init();
					SpawnedPistolgrip->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, MOD_PISTOL_GRIP_SOCKET_NAME);

					Pistolgrip = SpawnedPistolgrip;
				}
			}
			if (const TSubclassOf<AReciever>& RecieverClass = ModDataAsset->RecieverClass)
			{
				if (AReciever* SpawnedReciever = World->SpawnActor<AReciever>(RecieverClass))
				{
					SpawnedReciever->SetOwner(OwningActor);
					SpawnedReciever->Init();
					SpawnedReciever->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, MOD_RECIEVER_SOCKET_NAME);

					Reciever = SpawnedReciever;
				}
			}
			if (const TSubclassOf<ASightFront>& SightFrontClass = ModDataAsset->SightFrontClass)
			{
				if (ASightFront* SpawnedSightFront = World->SpawnActor<ASightFront>(SightFrontClass))
				{
					SpawnedSightFront->SetOwner(OwningActor);
					SpawnedSightFront->Init();
					SpawnedSightFront->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, MOD_SIGHT_FRONT_SOCKET_NAME);

					SightFront = SpawnedSightFront;
				}
			}
			if (const TSubclassOf<ASightRear>& SightRearClass = ModDataAsset->SightRearClass)
			{
				if (ASightRear* SpawnedSightRear = World->SpawnActor<ASightRear>(SightRearClass))
				{
					SpawnedSightRear->SetOwner(OwningActor);
					SpawnedSightRear->Init();
					SpawnedSightRear->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, MOD_SIGHT_REAR_SOCKET_NAME);

					SpawnedSightRear = SightRear;
				}
			}
			if (const TSubclassOf<AStock>& StockClass = ModDataAsset->StockClass)
			{
				if (AStock* SpawnedStock = World->SpawnActor<AStock>(StockClass))
				{
					SpawnedStock->SetOwner(OwningActor);
					SpawnedStock->Init();
					SpawnedStock->AttachToActor(OwningActor, FAttachmentTransformRules::KeepRelativeTransform, MOD_STOCK_SOCKET_NAME);

					Stock = SpawnedStock;
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
