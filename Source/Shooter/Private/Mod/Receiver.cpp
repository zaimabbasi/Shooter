// Fill out your copyright notice in the Description page of Project Settings.


#include "Mod/Receiver.h"
#include "Type/ShooterNameType.h"

AReceiver::AReceiver()
{
	
}

FName AReceiver::GetDefaultAttachParentSocketName() const
{
	return MOD_RECEIVER_SOCKET_NAME;
}

