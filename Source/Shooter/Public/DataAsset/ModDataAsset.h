// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ModDataAsset.generated.h"

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
};

UCLASS()
class SHOOTER_API UModDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FModData> ModDataArray;
	
};
