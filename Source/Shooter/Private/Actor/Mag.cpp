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

void AMag::AddAmmo(uint8 Count)
{
	Server_AddAmmo(Count);
}

void AMag::Server_AddAmmo_Implementation(uint8 Count)
{
	if (GetAmmoSpace() < Count)
	{
		return;
	}
	if (MagDataAsset)
	{
		if (UWorld* World = GetWorld())
		{
			for (uint8 Index = AmmoCount; Index < AmmoCount + Count; ++Index)
			{
				FName AmmoSocketName = FName(*FString::Printf(TEXT("patron_%03d"), Index + 1));
				if (Mesh && Mesh->DoesSocketExist(AmmoSocketName))
				{
					if (AAmmo* SpawnedAmmo = World->SpawnActor<AAmmo>(MagDataAsset->AmmoClass))
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

uint8 AMag::GetAmmoCapacity()
{
	return MagDataAsset == nullptr ? 0 : MagDataAsset->AmmoCapacity;
}

uint8 AMag::GetAmmoSpace()
{
	return GetAmmoCapacity() - AmmoCount;
}

