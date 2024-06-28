// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Shooter/Data/ModData.h"
#include "ModDataAsset.generated.h"

UCLASS()
class SHOOTER_API UModDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "ModData")
	TArray<FModData> ModDataArray;
	
};
