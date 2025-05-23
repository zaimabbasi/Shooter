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

FName AMag::GetDefaultAttachParentSocketName() const
{
	return MOD_MAGAZINE_SOCKET_NAME;
}

uint8 AMag::GetAmmoCapacity() const
{
	const UMagDataAsset* LoadedMagDataAsset = MagDataAsset.LoadSynchronous();
	return LoadedMagDataAsset ? LoadedMagDataAsset->AmmoCapacity : 0;
}

void AMag::AddAmmo(const uint8 Count)
{
	if (Count > 0 && Count <= (GetAmmoCapacity() - AmmoCount))
	{
		if (UWorld* World = GetWorld())
		{
			for (uint8 Index = AmmoCount + 1; Index <= AmmoCount + Count; ++Index)
			{
				const FName PatronSocketName = PATRON_SOCKET_NAME(Index);
				if (GetMesh() && GetMesh()->DoesSocketExist(PatronSocketName))
				{
					if (AAmmo* SpawnedAmmo = World->SpawnActor<AAmmo>(MagDataAsset.LoadSynchronous()->AmmoClass))
					{
						SpawnedAmmo->SetOwner(this);
						SpawnedAmmo->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, PatronSocketName);
						AmmoArray.Add(SpawnedAmmo);
					}
				}
			}
		}
		AmmoCount += Count;
	}
}

void AMag::PopAmmo()
{
	if (AmmoCount > 0)
	{
		AAmmo* PoppedAmmo = nullptr;
		const FName PatronSocketName = PATRON_SOCKET_NAME(AmmoCount);
		if (GetMesh() && GetMesh()->DoesSocketExist(PatronSocketName))
		{
			PoppedAmmo = AmmoArray.Pop(true);
		}
		--AmmoCount;
		OnMagAmmoPopped.Broadcast(PoppedAmmo);
	}
}
