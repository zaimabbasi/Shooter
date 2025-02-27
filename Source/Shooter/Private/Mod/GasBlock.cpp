// Fill out your copyright notice in the Description page of Project Settings.


#include "Mod/GasBlock.h"
#include "Types/ShooterNames.h"

AGasBlock::AGasBlock()
{

}

FName AGasBlock::GetDefaultAttachParentSocketName() const
{
    return MOD_GAS_BLOCK_SOCKET_NAME;
}

