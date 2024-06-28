#pragma once

#include "CoreMinimal.h"
#include "Shooter/Types/ModType.h"
#include "ShooterUtility.generated.h"

USTRUCT()
struct FShooterUtility
{
	GENERATED_BODY()

public:
	static FTransform TransformToBoneSpace(USkeletalMeshComponent* SkeletalMesh, FName BoneName, FTransform& WorldTransform)
	{
		if (SkeletalMesh == nullptr)
		{
			return FTransform();
		}
		FVector BoneSpaceOutLocation;
		FRotator BoneSpaceOutRotation;
		SkeletalMesh->TransformToBoneSpace(BoneName, WorldTransform.GetLocation(), FRotator(WorldTransform.GetRotation()), BoneSpaceOutLocation, BoneSpaceOutRotation);
		WorldTransform.SetLocation(BoneSpaceOutLocation);
		WorldTransform.SetRotation(FQuat(BoneSpaceOutRotation));

		return WorldTransform;
	}

	static FName GetModAttachmentSocketName(EModType ModType)
	{
		if (ModType == EModType::MT_Barrel)				return TEXT("mod_barrelSocket");
		else if (ModType == EModType::MT_Charge)		return TEXT("mod_chargeSocket");
		else if (ModType == EModType::MT_GasBlock)		return TEXT("mod_gas_blockSocket");
		else if (ModType == EModType::MT_Handguard)		return TEXT("mod_handguardSocket");
		else if (ModType == EModType::MT_Mag)			return TEXT("mod_magazineSocket");
		else if (ModType == EModType::MT_Muzzle)		return TEXT("mod_muzzleSocket");
		else if (ModType == EModType::MT_Pistolgrip)	return TEXT("mod_pistol_gripSocket");
		else if (ModType == EModType::MT_Reciever)		return TEXT("mod_recieverSocket");
		else if (ModType == EModType::MT_SightFront)	return TEXT("mod_sight_frontSocket");
		else if (ModType == EModType::MT_SightRear)		return TEXT("mod_sight_rearSocket");
		else if (ModType == EModType::MT_Stock)			return TEXT("mod_stockSocket");
		else											return TEXT("NAME_None");
	}
};