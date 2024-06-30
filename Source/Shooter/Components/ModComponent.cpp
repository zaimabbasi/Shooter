// Fill out your copyright notice in the Description page of Project Settings.


#include "ModComponent.h"
#include "Engine/SkeletalMeshSocket.h"
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
					AttachModToOwningActorSocket(SpawnedBarrel, TEXT("mod_barrelSocket"));
				}	
			}
			if (const TSubclassOf<ACharge>& ChargeClass = ModDataAsset->ChargeClass)
			{
				if (ACharge* SpawnedCharge = World->SpawnActor<ACharge>(ChargeClass))
				{
					SpawnedCharge->SetOwner(OwningActor);
					AttachModToOwningActorSocket(SpawnedCharge, TEXT("mod_chargeSocket"));
				}
			}
			if (const TSubclassOf<AGasBlock>& GasBlockClass = ModDataAsset->GasBlockClass)
			{
				if (AGasBlock* SpawnedGasBlock = World->SpawnActor<AGasBlock>(GasBlockClass))
				{
					SpawnedGasBlock->SetOwner(OwningActor);
					AttachModToOwningActorSocket(SpawnedGasBlock, TEXT("mod_gas_blockSocket"));
				}
			}
			if (const TSubclassOf<AHandguard>& HandguardClass = ModDataAsset->HandguardClass)
			{
				if (AHandguard* SpawnedHandguard = World->SpawnActor<AHandguard>(HandguardClass))
				{
					SpawnedHandguard->SetOwner(OwningActor);
					AttachModToOwningActorSocket(SpawnedHandguard, TEXT("mod_handguardSocket"));
				}
			}
			if (const TSubclassOf<AMag>& MagClass = ModDataAsset->MagClass)
			{
				if (AMag* SpawnedMag = World->SpawnActor<AMag>(MagClass))
				{
					SpawnedMag->SetOwner(OwningActor);
					AttachModToOwningActorSocket(SpawnedMag, TEXT("mod_magazineSocket"));
				}
			}
			if (const TSubclassOf<AMuzzle>& MuzzleClass = ModDataAsset->MuzzleClass)
			{
				if (AMuzzle* SpawnedMuzzle = World->SpawnActor<AMuzzle>(MuzzleClass))
				{
					SpawnedMuzzle->SetOwner(OwningActor);
					AttachModToOwningActorSocket(SpawnedMuzzle, TEXT("mod_muzzleSocket"));
				}
			}
			if (const TSubclassOf<APistolgrip>& PistolgripClass = ModDataAsset->PistolgripClass)
			{
				if (APistolgrip* SpawnedPistolgrip = World->SpawnActor<APistolgrip>(PistolgripClass))
				{
					SpawnedPistolgrip->SetOwner(OwningActor);
					AttachModToOwningActorSocket(SpawnedPistolgrip, TEXT("mod_pistol_gripSocket"));
				}
			}
			if (const TSubclassOf<AReciever>& RecieverClass = ModDataAsset->RecieverClass)
			{
				if (AReciever* SpawnedReciever = World->SpawnActor<AReciever>(RecieverClass))
				{
					SpawnedReciever->SetOwner(OwningActor);
					AttachModToOwningActorSocket(SpawnedReciever, TEXT("mod_recieverSocket"));
				}
			}
			if (const TSubclassOf<ASightFront>& SightFrontClass = ModDataAsset->SightFrontClass)
			{
				if (ASightFront* SpawnedSightFront = World->SpawnActor<ASightFront>(SightFrontClass))
				{
					SpawnedSightFront->SetOwner(OwningActor);
					AttachModToOwningActorSocket(SpawnedSightFront, TEXT("mod_sight_frontSocket"));
				}
			}
			if (const TSubclassOf<ASightRear>& SightRearClass = ModDataAsset->SightRearClass)
			{
				if (ASightRear* SpawnedSightRear = World->SpawnActor<ASightRear>(SightRearClass))
				{
					SpawnedSightRear->SetOwner(OwningActor);
					AttachModToOwningActorSocket(SpawnedSightRear, TEXT("mod_sight_rearSocket"));
				}
			}
			if (const TSubclassOf<AStock>& StockClass = ModDataAsset->StockClass)
			{
				if (AStock* SpawnedStock = World->SpawnActor<AStock>(StockClass))
				{
					SpawnedStock->SetOwner(OwningActor);
					AttachModToOwningActorSocket(SpawnedStock, TEXT("mod_stockSocket"));
				}
			}
		}
	}
}

void UModComponent::AttachModToOwningActorSocket(AMod* Mod, FName ModSocketName)
{
	if (OwningActorMesh == nullptr)
	{
		return;
	}
	if (OwningActorMesh->DoesSocketExist(ModSocketName))
	{
		if (const USkeletalMeshSocket* ModSocket = OwningActorMesh->GetSocketByName(ModSocketName))
		{
			ModSocket->AttachActor(Mod, OwningActorMesh);
		}
	}
}
