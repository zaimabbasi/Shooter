// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/GasBlock.h"
#include "Type/ShooterNameType.h"

AGasBlock::AGasBlock()
{

}

FName AGasBlock::GetDefaultAttachParentSocketName() const
{
    return MOD_GAS_BLOCK_SOCKET_NAME;
}

