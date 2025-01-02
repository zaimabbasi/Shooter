// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Mount.h"
#include "Type/ShooterNameType.h"

AMount::AMount()
{

}

FName AMount::GetDefaultAttachParentSocketName() const
{
    return MOD_MOUNT_003_SOCKET_NAME;
}
