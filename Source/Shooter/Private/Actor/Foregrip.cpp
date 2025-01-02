// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Foregrip.h"
#include "Type/ShooterNameType.h"

AForegrip::AForegrip()
{

}

FName AForegrip::GetDefaultAttachParentSocketName() const
{
    return MOD_FOREGRIP_SOCKET_NAME;
}
