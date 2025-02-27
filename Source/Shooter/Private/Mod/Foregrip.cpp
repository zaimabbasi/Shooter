// Fill out your copyright notice in the Description page of Project Settings.


#include "Mod/Foregrip.h"
#include "Types/ShooterNames.h"

AForegrip::AForegrip()
{

}

FName AForegrip::GetDefaultAttachParentSocketName() const
{
    return MOD_FOREGRIP_SOCKET_NAME;
}
