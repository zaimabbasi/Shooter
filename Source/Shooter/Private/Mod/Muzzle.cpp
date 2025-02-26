// Fill out your copyright notice in the Description page of Project Settings.


#include "Mod/Muzzle.h"
#include "Type/ShooterNameType.h"

AMuzzle::AMuzzle()
{
	
}

FName AMuzzle::GetDefaultAttachParentSocketName() const
{
	return MOD_MUZZLE_SOCKET_NAME;
}

