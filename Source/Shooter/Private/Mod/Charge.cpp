// Fill out your copyright notice in the Description page of Project Settings.


#include "Mod/Charge.h"
#include "Type/ShooterNameType.h"

ACharge::ACharge()
{

}

FName ACharge::GetDefaultAttachParentSocketName() const
{
    return MOD_CHARGE_SOCKET_NAME;
}
