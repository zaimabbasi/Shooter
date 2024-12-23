// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Barrel.h"
#include "Type/ShooterNameType.h"

ABarrel::ABarrel()
{
	
}

FName ABarrel::GetDefaultAttachParentSocketName() const
{
	return MOD_BARREL_SOCKET_NAME;
}

