// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AmmoDataAsset.generated.h"

UCLASS()
class SHOOTER_API UAmmoDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	USkeletalMesh* FullMesh;

	UPROPERTY(EditAnywhere)
	USkeletalMesh* ShellMesh;

};
