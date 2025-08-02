#pragma once

#include "CoreMinimal.h"
#include "Engine/SkeletalMeshSocket.h"
#include "NiagaraFunctionLibrary.h"
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

	static UNiagaraComponent* SpawnNiagaraSystemAttached(UNiagaraSystem* SystemTemplate, USceneComponent* AttachToComponent, FName AttachPointName, bool bAutoDestroy)
	{
		return UNiagaraFunctionLibrary::SpawnSystemAttached(SystemTemplate,
			AttachToComponent,
			AttachPointName,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			bAutoDestroy);
	}
};
