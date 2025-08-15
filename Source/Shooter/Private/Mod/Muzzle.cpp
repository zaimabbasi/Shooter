// Fill out your copyright notice in the Description page of Project Settings.


#include "Mod/Muzzle.h"
#include "Types/ShooterNames.h"
#include "Utility/ShooterUtility.h"

AMuzzle::AMuzzle()
{
	
}

FName AMuzzle::GetDefaultAttachParentSocketName() const
{
	return MOD_MUZZLE_SOCKET_NAME;
}

void AMuzzle::SpawnMuzzleFlashEffect(UNiagaraSystem* MuzzleFlashSystem) const
{
	FShooterUtility::SpawnNiagaraSystemAttached(MuzzleFlashSystem, GetMesh(), MUZZLEFLASH_SOCKET_NAME(0), true);

	// Disable for now until parameters can be set somehow
	/*TArray<FName> BoneNames;
	if (GetMesh())
	{
		GetMesh()->GetBoneNames(BoneNames);
	}

	for (FName BoneName : BoneNames)
	{
		if (BoneName.ToString().Contains(MUZZLEFLASH_NAME))
		{
			FShooterUtility::SpawnNiagaraSystemAttached(MuzzleFlashSystem, GetMesh(), BoneName, true);
		}
	}*/
}

FTransform AMuzzle::GetMuzzleFlashSocketTransform() const
{
	FTransform MuzzleFlashSocketTransform;
	if (GetMesh())
	{
		MuzzleFlashSocketTransform = GetMesh()->GetSocketTransform(MUZZLEFLASH_SOCKET_NAME(0));
	}
	return MuzzleFlashSocketTransform;
}

//UNiagaraComponent* AMuzzle::SpawnMuzzleSmokeEffect(UNiagaraSystem* MuzzleSmokeSystem) const
//{
//	return FShooterUtility::SpawnNiagaraSystemAttached(MuzzleSmokeSystem, GetMesh(), MUZZLEFLASH_SOCKET_NAME(0), true);
//}
