// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Handguard.h"
#include "Type/ShooterNameType.h"

AHandguard::AHandguard()
{
	
}

FName AHandguard::GetDefaultAttachParentSocketName() const
{
	return MOD_HANDGUARD_SOCKET_NAME;
}

