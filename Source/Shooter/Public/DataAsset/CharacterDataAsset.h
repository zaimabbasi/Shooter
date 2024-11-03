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

USTRUCT()
struct FMovementParams
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float MaxWalkSpeed;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float MaxWalkSpeedCrouched;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float MaxWalkSpeedCrouchedSlow;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float MaxWalkSpeedProned;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float MaxWalkSpeedSlow;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float MaxWalkSpeedSprint;

};



UCLASS()
class SHOOTER_API UCharacterDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "InventoryParams")
	FInventoryParams InventoryParams;

	UPROPERTY(EditAnywhere, Category = "MovementParams")
	FMovementParams MovementParams;

};
