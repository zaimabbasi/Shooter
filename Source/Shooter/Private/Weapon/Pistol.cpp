// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Pistol.h"
#include "Types/ShooterNames.h"

APistol::APistol()
{
	
}

void APistol::OnInventoryIndexChanged()
{
	HolsterSocketName = InventoryIndex == INDEX_NONE ? FName(NAME_None) : PISTOL_HOLSTER_SOCKET_NAME;
}
