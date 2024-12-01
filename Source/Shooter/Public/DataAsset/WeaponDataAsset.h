// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Enum/WeaponFiremode.h"
#include "WeaponDataAsset.generated.h"

class UModDataAsset;

UCLASS()
class SHOOTER_API UWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TArray<EWeaponFiremode> Firemodes;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	bool IsOneHanded;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	uint16 RateOfFire;

	UPROPERTY(EditAnywhere, Category = "DataAsset")
	TSoftObjectPtr<UModDataAsset> ModDataAsset;

};
