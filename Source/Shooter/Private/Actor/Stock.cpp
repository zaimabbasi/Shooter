// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Stock.h"
#include "Type/ShooterNameType.h"

AStock::AStock()
{
	
}

FName AStock::GetDefaultAttachParentSocketName() const
{
	return MOD_STOCK_SOCKET_NAME;
}

