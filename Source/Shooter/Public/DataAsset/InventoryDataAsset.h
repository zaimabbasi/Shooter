// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InventoryDataAsset.generated.h"

class AWeapon;

UCLASS()
class SHOOTER_API UInventoryDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (DisallowedClasses = "Pistol"))
	TSubclassOf<AWeapon> PrimaryWeaponClass;

	UPROPERTY(EditAnywhere)
	uint8 PrimaryWeaponAmmo;

	UPROPERTY(EditAnywhere, meta = (DisallowedClasses = "AssaultRifle"))
	TSubclassOf<AWeapon> SecondaryWeaponClass;

	UPROPERTY(EditAnywhere)
	uint8 SecondaryWeaponAmmo;

};
