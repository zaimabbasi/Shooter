// Fill out your copyright notice in the Description page of Project Settings.


#include "Mod/Barrel.h"
#include "Types/ShooterNames.h"

ABarrel::ABarrel()
{
	
}

FName ABarrel::GetDefaultAttachParentSocketName() const
{
	return MOD_BARREL_SOCKET_NAME;
}

