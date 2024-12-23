// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/SightFront.h"
#include "Type/ShooterNameType.h"

ASightFront::ASightFront()
{
	
}

FName ASightFront::GetDefaultAttachParentSocketName() const
{
	return MOD_SIGHT_FRONT_SOCKET_NAME;
}

