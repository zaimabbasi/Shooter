// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterDataAsset.generated.h"

class AWeapon;

USTRUCT()
struct FInventoryData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> WeaponClass;
};

UCLASS()
class SHOOTER_API UCharacterDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "InventoryData")
	TArray<FInventoryData> InventoryDataArray;
	
};
