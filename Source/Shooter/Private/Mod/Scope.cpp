// Fill out your copyright notice in the Description page of Project Settings.


#include "Mod/Scope.h"
#include "Types/ShooterNames.h"

AScope::AScope()
{

}

void AScope::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if (HasModes())
    {
        SetMode(2);
    }
}

FName AScope::GetDefaultAttachParentSocketName() const
{
    return Super::GetDefaultAttachParentSocketName();
}

bool AScope::HasModes() const
{
    return GetMesh() && GetMesh()->DoesSocketExist(MOD_TACTICAL_SOCKET_NAME);
}

uint8 AScope::GetNumModes() const
{
    return GetModeSocketNames().Num();
}

void AScope::SetMode(uint8 Mode)
{
    if (GetMesh())
    {
        uint8 NumModes = GetNumModes();
        for (uint8 Index = 0; Index < NumModes; ++Index)
        {
            if (Index == Mode)
            {
                GetMesh()->UnHideBoneByName(MODE_SOCKET_NAME(Index));
            }
            else
            {
                GetMesh()->HideBoneByName(MODE_SOCKET_NAME(Index), EPhysBodyOp::PBO_None);
            }
        }
    }
}

TArray<FName> AScope::GetModeSocketNames() const
{
    TArray<FName> AllSocketNames;
    if (GetMesh())
    {
        AllSocketNames = GetMesh()->GetAllSocketNames();
    }

    return AllSocketNames.FilterByPredicate([&](const FName& SocketName)
        {
            return SocketName.ToString().Contains("mode");
        });
}
