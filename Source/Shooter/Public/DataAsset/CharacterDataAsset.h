// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterDataAsset.generated.h"

class AWeapon;

USTRUCT()
struct FInventoryParams
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (DisallowedClasses = "Pistol"))
	TSubclassOf<AWeapon> PrimaryWeaponClass;

	UPROPERTY(EditAnywhere)
	uint8 PrimaryWeaponMaxAmmo;

	UPROPERTY(EditAnywhere, meta = (DisallowedClasses = "AssaultRifle"))
	TSubclassOf<AWeapon> SecondaryWeaponClass;

	UPROPERTY(EditAnywhere)
	uint8 SecondaryWeaponMaxAmmo;
};

UCLASS()
class SHOOTER_API UCharacterDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "InventoryParams")
	FInventoryParams InventoryParams;

};
