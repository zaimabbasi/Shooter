// Fill out your copyright notice in the Description page of Project Settings.


#include "Mod/Stock.h"
#include "Types/ShooterNames.h"

AStock::AStock()
{
	
}

FName AStock::GetDefaultAttachParentSocketName() const
{
	return MOD_STOCK_SOCKET_NAME;
}

