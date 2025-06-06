// Fill out your copyright notice in the Description page of Project Settings.


#include "Mod/Mag.h"
#include "Net/UnrealNetwork.h"
#include "Ammo/Ammo.h"
#include "Data/MagDataAsset.h"
#include "Types/ShooterNames.h"

AMag::AMag()
{	
}

void AMag::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AMag, AmmoCount, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AMag, AmmoArray, COND_OwnerOnly);

}

void AMag::Init()
{
	Super::Init();

	if (MagDataAsset.LoadSynchronous())
	{
		if (UWorld* World = GetWorld())
		{
			for (uint8 Index = 0; Index < MagDataAsset->AmmoCapacity; ++Index)
			{
				if (AAmmo* SpawnedAmmo = World->SpawnActor<AAmmo>(MagDataAsset->AmmoClass))
				{
					SpawnedAmmo->SetOwner(this);
					SpawnedAmmo->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, PATRON_SOCKET_NAME(Index + 1));
					SpawnedAmmo->SetActorHiddenInGame(true);

					AmmoArray.Add(SpawnedAmmo);
				}
			}
		}
	}
}

FName AMag::GetDefaultAttachParentSocketName() const
{
	return MOD_MAGAZINE_SOCKET_NAME;
}

uint8 AMag::GetAmmoCapacity() const
{
	return MagDataAsset.LoadSynchronous() != nullptr ? MagDataAsset->AmmoCapacity : 0;
}

uint8 AMag::GetAmmoSpace() const
{
	return GetAmmoCapacity() - AmmoCount;
}

void AMag::AddAmmo(uint8 Count)
{
	if (Count > 0 && Count <= GetAmmoSpace())
	{
		AmmoCount += Count;

		for (uint8 Index = AmmoCount - Count; Index < AmmoCount; ++Index)
		{
			if (AAmmo* Ammo = GetAmmoAtIndex(Index))
			{
				bool bDoesSocketExists = GetMesh() && GetMesh()->DoesSocketExist(PATRON_SOCKET_NAME(Index + 1));
				Ammo->SetActorHiddenInGame(!bDoesSocketExists);
			}
		}
	}
}

AAmmo* AMag::PopAmmo()
{
	AAmmo* PoppedAmmo = nullptr;
	if (AmmoCount > 0)
	{
		--AmmoCount;
		PoppedAmmo = GetAmmoAtIndex(AmmoCount);
	}
	return PoppedAmmo;
}

AAmmo* AMag::GetAmmoAtIndex(uint8 AmmoIndex) const
{
	return AmmoArray.IsValidIndex(AmmoIndex) ? AmmoArray[AmmoIndex] : nullptr;
}
