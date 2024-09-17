// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Mag.h"
#include "Net/UnrealNetwork.h"
#include "Actor/Ammo.h"
#include "DataAsset/MagDataAsset.h"
#include "Type/ShooterNameType.h"

AMag::AMag()
{
	
}

void AMag::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMag, AmmoCount);
	DOREPLIFETIME(AMag, AmmoArray);

}

void AMag::Server_AddAmmo_Implementation(const uint8 Count)
{
	if (GetAmmoSpace() >= Count)
	{
		if (UWorld* World = GetWorld())
		{
			for (uint8 Index = AmmoCount + 1; Index < AmmoCount + Count; ++Index)
			{
				const FName PatronSocketName = PATRON_SOCKET_NAME(Index);
				if (Mesh && Mesh->DoesSocketExist(PatronSocketName))
				{
					if (AAmmo* SpawnedAmmo = World->SpawnActor<AAmmo>(MagDataAsset.LoadSynchronous()->AmmoClass))
					{
						SpawnedAmmo->SetOwner(this);
						SpawnedAmmo->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, PatronSocketName);
						AmmoArray.Add(SpawnedAmmo);
					}
				}
			}
		}
		AmmoCount += Count;
	}
}

void AMag::Server_PopAmmo_Implementation()
{
	if (AmmoCount > 0)
	{
		AAmmo* PoppedAmmo = nullptr;
		const FName PatronSocketName = PATRON_SOCKET_NAME(AmmoCount);
		if (Mesh && Mesh->DoesSocketExist(PatronSocketName))
		{
			PoppedAmmo = AmmoArray.Pop(true);
		}
		--AmmoCount;
		OnMagAmmoPopped.Broadcast(PoppedAmmo);
	}
}

uint8 AMag::GetAmmoCapacity()
{
	return MagDataAsset == nullptr ? 0 : MagDataAsset->AmmoCapacity;
}

uint8 AMag::GetAmmoSpace()
{
	return GetAmmoCapacity() - AmmoCount;
}
