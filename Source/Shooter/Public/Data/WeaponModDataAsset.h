// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponModDataAsset.generated.h"

class AMod;

USTRUCT()
struct FModData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FName AttachParentSocketName;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AMod> ModClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AMod> ModParentClass;

};

UCLASS()
class SHOOTER_API UWeaponModDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FModData> ModDataArray;

};