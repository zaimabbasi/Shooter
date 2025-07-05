// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponDataAsset.generated.h"

class UWeaponModDataAsset;
enum class EWeaponFiremode : uint8;

UCLASS()
class SHOOTER_API UWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TArray<EWeaponFiremode> Firemodes;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	bool IsOneHanded;

	UPROPERTY(EditAnywhere, Category = "Performance", meta = (ForceUnits = "rpm"))
	uint16 RateOfFire;

	UPROPERTY(EditAnywhere, Category = "Performance|Recoil", meta = (UIMin = "0", UIMax = "15", ClampMin = "0", ClampMax = "15"))
	float VerticalKick;

	UPROPERTY(EditAnywhere, Category = "Performance|Recoil", meta = (UIMin = "0", UIMax = "5", ClampMin = "0", ClampMax = "5"))
	float HorizontalSpread;

	UPROPERTY(EditAnywhere, Category = "Performance|Recoil", meta = (UIMin = "-1", UIMax = "1", ClampMin = "-1", ClampMax = "1"))
	float HorizontalSpreadDirection;

	UPROPERTY(EditAnywhere, Category = "DataAsset")
	TSoftObjectPtr<UWeaponModDataAsset> WeaponModDataAsset;

};
