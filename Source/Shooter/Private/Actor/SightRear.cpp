// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/SightRear.h"
#include "Type/ShooterNameType.h"

ASightRear::ASightRear()
{
	
}

FName ASightRear::GetDefaultAttachParentSocketName() const
{
	return MOD_SIGHT_REAR_SOCKET_NAME;
}

