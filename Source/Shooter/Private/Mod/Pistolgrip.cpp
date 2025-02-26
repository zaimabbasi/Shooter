// Fill out your copyright notice in the Description page of Project Settings.


#include "Mod/Pistolgrip.h"
#include "Type/ShooterNameType.h"

APistolgrip::APistolgrip()
{
	
}

FName APistolgrip::GetDefaultAttachParentSocketName() const
{
	return MOD_PISTOL_GRIP_SOCKET_NAME;
}

