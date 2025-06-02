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

	if (MagDataAsset.LoadSynchronous() == nullptr)
	{
		return;
	}
	if (UWorld* World = GetWorld())
	{
		for (uint8 Index = 0; Index < MagDataAsset->AmmoCapacity; ++Index)
		{
			if (AAmmo* SpawnedAmmo = World->SpawnActor<AAmmo>(MagDataAsset->AmmoClass))
			{
				SpawnedAmmo->SetOwner(this);
				SpawnedAmmo->SetActorHiddenInGame(true);

				AmmoArray.Add(SpawnedAmmo);
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

//void AMag::AddAmmo(const uint8 Count)
//{
//	if (Count > 0 && Count <= (GetAmmoCapacity() - AmmoCount))
//	{
//		if (UWorld* World = GetWorld())
//		{
//			for (uint8 Index = AmmoCount + 1; Index <= AmmoCount + Count; ++Index)
//			{
//				const FName PatronSocketName = PATRON_SOCKET_NAME(Index);
//				if (GetMesh() && GetMesh()->DoesSocketExist(PatronSocketName))
//				{
//					if (AAmmo* SpawnedAmmo = World->SpawnActor<AAmmo>(MagDataAsset.LoadSynchronous()->AmmoClass))
//					{
//						SpawnedAmmo->SetOwner(this);
//						SpawnedAmmo->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, PatronSocketName);
//						AmmoArray.Add(SpawnedAmmo);
//					}
//				}
//			}
//		}
//		AmmoCount += Count;
//	}
//}
void AMag::AddAmmo(uint8 Count)
{
	if (Count > 0 && Count <= GetAmmoSpace())
	{
		for (uint8 Index = AmmoCount; Index < AmmoCount + Count; ++Index)
		{
			if (AAmmo* Ammo = GetAmmoAtIndex(Index))
			{
				if (Ammo->GetIsEmpty())
				{
					Ammo->SetIsEmpty(false);	
				}
				if (Ammo->GetMesh())
				{
					Ammo->GetMesh()->SetSimulatePhysics(false);
					Ammo->GetMesh()->SetEnableGravity(false);
					Ammo->GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				}

				FName PatronSocketName = PATRON_SOCKET_NAME(Index + 1);
				if (GetMesh())
				{
					Ammo->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, PatronSocketName);
					
					bool bDoesSocketExists = GetMesh()->DoesSocketExist(PatronSocketName);
					Ammo->SetActorHiddenInGame(!bDoesSocketExists);
				}
			}
		}
		AmmoCount += Count;
	}
}

//void AMag::PopAmmo()
//{
//	if (AmmoCount > 0)
//	{
//		AAmmo* PoppedAmmo = nullptr;
//		const FName PatronSocketName = PATRON_SOCKET_NAME(AmmoCount);
//		if (GetMesh() && GetMesh()->DoesSocketExist(PatronSocketName))
//		{
//			PoppedAmmo = AmmoArray.Pop(true);
//		}
//		--AmmoCount;
//		OnMagAmmoPopped.Broadcast(PoppedAmmo);
//	}
//}
void AMag::PopAmmo()
{
	if (AmmoCount > 0)
	{
		--AmmoCount;
		OnMagAmmoPopped.Broadcast(GetAmmoAtIndex(AmmoCount));
	}
}

AAmmo* AMag::GetAmmoAtIndex(uint8 AmmoIndex) const
{
	return AmmoArray.IsValidIndex(AmmoIndex) ? AmmoArray[AmmoIndex] : nullptr;
}
