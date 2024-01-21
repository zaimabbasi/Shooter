// Fill out your copyright notice in the Description page of Project Settings.


#include "ModComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "Shooter/Mod/Barrel.h"
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
	//DOREPLIFETIME(UModComponent, Flashlight);
	//DOREPLIFETIME(UModComponent, Foregrip);
	DOREPLIFETIME(UModComponent, GasBlock);
	DOREPLIFETIME(UModComponent, Handguard);
	DOREPLIFETIME(UModComponent, Mag);
	//DOREPLIFETIME(UModComponent, Mount);
	DOREPLIFETIME(UModComponent, Muzzle);
	DOREPLIFETIME(UModComponent, Pistolgrip);
	DOREPLIFETIME(UModComponent, Reciever);
	//DOREPLIFETIME(UModComponent, Scope);
	DOREPLIFETIME(UModComponent, SightFront);
	DOREPLIFETIME(UModComponent, SightRear);
	DOREPLIFETIME(UModComponent, Stock);
	//DOREPLIFETIME(UModComponent, Tactical);
}

void UModComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (OwningWeapon && OwningWeapon->HasAuthority())
	{
		if (UWorld* World = GetWorld())
		{
			if (const TSubclassOf<ABarrel>& BarrelClass = ModData.BarrelClass)
			{
				Barrel = World->SpawnActor<ABarrel>(BarrelClass);
				Barrel->SetOwner(OwningWeapon);
			}
			/*if (const TSubclassOf<AFlashlight>& FlashlightClass = ModData.FlashlightClass)
			{
				Flashlight = World->SpawnActor<AFlashlight>(FlashlightClass);
				Flashlight->SetOwner(OwningWeapon);
			}*/
			/*if (const TSubclassOf<AForegrip>& ForegripClass = ModData.ForegripClass)
			{
				Foregrip = World->SpawnActor<AForegrip>(ForegripClass);
				Foregrip->SetOwner(OwningWeapon);
			}*/
			if (const TSubclassOf<AGasBlock>& GasBlockClass = ModData.GasBlockClass)
			{
				GasBlock = World->SpawnActor<AGasBlock>(GasBlockClass);
				GasBlock->SetOwner(OwningWeapon);
			}
			if (const TSubclassOf<AHandguard>& HandguardClass = ModData.HandguardClass)
			{
				Handguard = World->SpawnActor<AHandguard>(HandguardClass);
				Handguard->SetOwner(OwningWeapon);
			}
			if (const TSubclassOf<AMag>& MagClass = ModData.MagClass)
			{
				Mag = World->SpawnActor<AMag>(MagClass);
				Mag->SetOwner(OwningWeapon);
			}
			/*if (const TSubclassOf<AMount>& MountClass = ModData.MountClass)
			{
				Mount = World->SpawnActor<AMount>(MountClass);
				Mount->SetOwner(OwningWeapon);
			}*/
			if (const TSubclassOf<AMuzzle>& MuzzleClass = ModData.MuzzleClass)
			{
				Muzzle = World->SpawnActor<AMuzzle>(MuzzleClass);
				Muzzle->SetOwner(OwningWeapon);
			}
			if (const TSubclassOf<APistolgrip>& PistolgripClass = ModData.PistolgripClass)
			{
				Pistolgrip = World->SpawnActor<APistolgrip>(PistolgripClass);
				Pistolgrip->SetOwner(OwningWeapon);
			}
			if (const TSubclassOf<AReciever>& RecieverClass = ModData.RecieverClass)
			{
				Reciever = World->SpawnActor<AReciever>(RecieverClass);
				Reciever->SetOwner(OwningWeapon);
			}
			/*if (const TSubclassOf<AScope>& ScopeClass = ModData.ScopeClass)
			{
				Scope = World->SpawnActor<AScope>(ScopeClass);
				Scope->SetOwner(OwningWeapon);
			}*/
			if (const TSubclassOf<ASightFront>& SightFrontClass = ModData.SightFrontClass)
			{
				SightFront = World->SpawnActor<ASightFront>(SightFrontClass);
				SightFront->SetOwner(OwningWeapon);
			}
			if (const TSubclassOf<ASightRear>& SightRearClass = ModData.SightRearClass)
			{
				SightRear = World->SpawnActor<ASightRear>(SightRearClass);
				SightRear->SetOwner(OwningWeapon);
			}
			if (const TSubclassOf<AStock>& StockClass = ModData.StockClass)
			{
				Stock = World->SpawnActor<AStock>(StockClass);
				Stock->SetOwner(OwningWeapon);
			}
			/*if (const TSubclassOf<ATactical>& TacticalClass = ModData.TacticalClass)
			{
				Tactical = World->SpawnActor<ATactical>(TacticalClass);
				Tactical->SetOwner(OwningWeapon);
			}*/
		}
		if (Barrel)
		{
			FName BarrelSocketName = TEXT("mod_barrelSocket");
			if (USkeletalMeshComponent* WeaponMesh = OwningWeapon->GetMesh())
			{
				if (WeaponMesh->DoesSocketExist(BarrelSocketName))
				{
					if (const USkeletalMeshSocket* BarrelSocket = WeaponMesh->GetSocketByName(BarrelSocketName))
					{
						BarrelSocket->AttachActor(Barrel, WeaponMesh);
					}
				}
			}
		}
		if (GasBlock)
		{
			FName GasBlockSocketName = TEXT("mod_gas_blockSocket");
			if (USkeletalMeshComponent* WeaponMesh = OwningWeapon->GetMesh())
			{
				if (WeaponMesh->DoesSocketExist(GasBlockSocketName))
				{
					if (const USkeletalMeshSocket* GasBlockSocket = WeaponMesh->GetSocketByName(GasBlockSocketName))
					{
						GasBlockSocket->AttachActor(GasBlock, WeaponMesh);
					}
				}
			}
		}
		if (Handguard)
		{
			FName HandguardSocketName = TEXT("mod_handguardSocket");
			if (USkeletalMeshComponent* WeaponMesh = OwningWeapon->GetMesh())
			{
				if (WeaponMesh->DoesSocketExist(HandguardSocketName))
				{
					if (const USkeletalMeshSocket* HandguardSocket = WeaponMesh->GetSocketByName(HandguardSocketName))
					{
						HandguardSocket->AttachActor(Handguard, WeaponMesh);
					}
				}
			}
		}
		if (Mag)
		{
			FName MagSocketName = TEXT("mod_magazineSocket");
			if (USkeletalMeshComponent* WeaponMesh = OwningWeapon->GetMesh())
			{
				if (WeaponMesh->DoesSocketExist(MagSocketName))
				{
					if (const USkeletalMeshSocket* MagSocket = WeaponMesh->GetSocketByName(MagSocketName))
					{
						MagSocket->AttachActor(Mag, WeaponMesh);
					}
				}
			}
		}
		if (Muzzle)
		{
			FName MuzzleSocketName = TEXT("mod_muzzleSocket");
			if (USkeletalMeshComponent* WeaponMesh = OwningWeapon->GetMesh())
			{
				if (WeaponMesh->DoesSocketExist(MuzzleSocketName))
				{
					if (const USkeletalMeshSocket* MuzzleSocket = WeaponMesh->GetSocketByName(MuzzleSocketName))
					{
						MuzzleSocket->AttachActor(Muzzle, WeaponMesh);
					}
				}
			}
		}
		if (Pistolgrip)
		{
			FName PistolgripSocketName = TEXT("mod_pistol_gripSocket");
			if (USkeletalMeshComponent* WeaponMesh = OwningWeapon->GetMesh())
			{
				if (WeaponMesh->DoesSocketExist(PistolgripSocketName))
				{
					if (const USkeletalMeshSocket* PistolgripSocket = WeaponMesh->GetSocketByName(PistolgripSocketName))
					{
						PistolgripSocket->AttachActor(Pistolgrip, WeaponMesh);
					}
				}
			}
		}
		if (Reciever)
		{
			FName RecieverSocketName = TEXT("mod_recieverSocket");
			if (USkeletalMeshComponent* WeaponMesh = OwningWeapon->GetMesh())
			{
				if (WeaponMesh->DoesSocketExist(RecieverSocketName))
				{
					if (const USkeletalMeshSocket* RecieverSocket = WeaponMesh->GetSocketByName(RecieverSocketName))
					{
						RecieverSocket->AttachActor(Reciever, WeaponMesh);
					}
				}
			}
		}
		if (SightFront)
		{
			FName SightFrontSocketName = TEXT("mod_sight_frontSocket");
			if (USkeletalMeshComponent* WeaponMesh = OwningWeapon->GetMesh())
			{
				if (WeaponMesh->DoesSocketExist(SightFrontSocketName))
				{
					if (const USkeletalMeshSocket* SightFrontSocket = WeaponMesh->GetSocketByName(SightFrontSocketName))
					{
						SightFrontSocket->AttachActor(SightFront, WeaponMesh);
					}
				}
				else if (Reciever)
				{
					if (USkeletalMeshComponent* RecieverMesh = Reciever->GetMesh())
					{
						if (const USkeletalMeshSocket* SightFrontSocket = RecieverMesh->GetSocketByName(SightFrontSocketName))
						{
							SightFrontSocket->AttachActor(SightFront, RecieverMesh);
						}
					}
				}
			}
		}
		if (SightRear)
		{
			FName SightRearSocketName = TEXT("mod_sight_rearSocket");
			if (USkeletalMeshComponent* WeaponMesh = OwningWeapon->GetMesh())
			{
				if (WeaponMesh->DoesSocketExist(SightRearSocketName))
				{
					if (const USkeletalMeshSocket* SightRearSocket = WeaponMesh->GetSocketByName(SightRearSocketName))
					{
						SightRearSocket->AttachActor(SightRear, WeaponMesh);
					}
				}
				else if (Reciever)
				{
					if (USkeletalMeshComponent* RecieverMesh = Reciever->GetMesh())
					{
						if (const USkeletalMeshSocket* SightRearSocket = RecieverMesh->GetSocketByName(SightRearSocketName))
						{
							SightRearSocket->AttachActor(SightRear, RecieverMesh);
						}
					}
				}
			}
		}
		if (Stock)
		{
			FName StockSocketName = TEXT("mod_stockSocket");
			if (USkeletalMeshComponent* WeaponMesh = OwningWeapon->GetMesh())
			{
				if (WeaponMesh->DoesSocketExist(StockSocketName))
				{
					if (const USkeletalMeshSocket* StockSocket = WeaponMesh->GetSocketByName(StockSocketName))
					{
						StockSocket->AttachActor(Stock, WeaponMesh);
					}
				}
			}
		}
	}

}
