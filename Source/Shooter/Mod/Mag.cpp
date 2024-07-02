// Fill out your copyright notice in the Description page of Project Settings.


#include "Mag.h"
#include "Net/UnrealNetwork.h"
#include "Shooter/Data/MagDataAsset.h"

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
	/*if (GetAmmoSpace() < Count)
	{
		return false;
	}
	AmmoCount += Count;
	return true;*/
}

void AMag::Server_AddAmmo_Implementation(uint8 Count)
{
	if (GetAmmoSpace() < Count)
	{
		return;
	}
	AmmoCount += Count;
	if (MagDataAsset)
	{
		
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

