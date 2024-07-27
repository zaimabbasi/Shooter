// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Mag.h"
#include "Net/UnrealNetwork.h"
#include "Actor/Ammo.h"
#include "DataAsset/MagDataAsset.h"

AMag::AMag()
{
	
}

void AMag::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMag, AmmoCount);

}

void AMag::Server_RemoveAmmo_Implementation()
{
	if (AmmoCount == 0)
	{
		return;
	}
	FName AmmoSocketName = FName(*FString::Printf(TEXT("patron_%03d"), AmmoCount));
	AAmmo* FoundAmmo = AmmoMap.FindRef(AmmoSocketName);
	if (FoundAmmo)
	{
		AmmoMap.Remove(AmmoSocketName);
	}
	--AmmoCount;
	OnAmmoRemovedDelegate.Broadcast(FoundAmmo);
}

void AMag::Server_AddAmmo_Implementation(const uint8 Count)
{
	if (GetAmmoSpace() < Count)
	{
		return;
	}
	if (const UMagDataAsset* LoadedMagDataAsset = MagDataAsset.LoadSynchronous())
	{
		if (UWorld* World = GetWorld())
		{
			for (uint8 Index = AmmoCount; Index < AmmoCount + Count; ++Index)
			{
				FName AmmoSocketName = FName(*FString::Printf(TEXT("patron_%03d"), Index + 1));
				if (Mesh && Mesh->DoesSocketExist(AmmoSocketName))
				{
					if (AAmmo* SpawnedAmmo = World->SpawnActor<AAmmo>(LoadedMagDataAsset->AmmoClass))
					{
						SpawnedAmmo->SetOwner(this);
						SpawnedAmmo->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform, AmmoSocketName);
						AmmoMap.Add(AmmoSocketName, SpawnedAmmo);
					}
				}
			}
		}
	}
	AmmoCount += Count;
}

void AMag::AddAmmo(const uint8 Count)
{
	if (GetAmmoSpace() < Count)
	{
		return;
	}
	Server_AddAmmo(Count);
}

void AMag::RemoveAmmo()
{
	if (AmmoCount == 0)
	{
		return;
	}
	Server_RemoveAmmo();
}

uint8 AMag::GetAmmoCapacity()
{
	return MagDataAsset == nullptr ? 0 : MagDataAsset->AmmoCapacity;
}

uint8 AMag::GetAmmoSpace()
{
	return GetAmmoCapacity() - AmmoCount;
}
