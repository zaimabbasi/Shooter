// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponDataAsset.generated.h"

class ABarrel;
//class AFlashlight;
//class AForegrip;
class AGasBlock;
class AHandguard;
class AMag;
//class AMount;
class AMuzzle;
class AReciever;
class APistolgrip;
//class AScope;
class ASightFront;
class ASightRear;
class AStock;
//class ATactical;

USTRUCT()
struct FModData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABarrel> BarrelClass;

	/*UPROPERTY(EditAnywhere)
	TSubclassOf<AFlashlight> FlashlightClass;*/

	/*UPROPERTY(EditAnywhere)
	TSubclassOf<AForegrip> ForegripClass;*/

	UPROPERTY(EditAnywhere)
	TSubclassOf<AGasBlock> GasBlockClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AHandguard> HandguardClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AMag> MagClass;

	/*UPROPERTY(EditAnywhere)
	TSubclassOf<AMount> MountClass;*/

	UPROPERTY(EditAnywhere)
	TSubclassOf<AMuzzle> MuzzleClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AReciever> RecieverClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<APistolgrip> PistolgripClass;

	/*UPROPERTY(EditAnywhere)
	TSubclassOf<AScope> ScopeClass;*/

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASightFront> SightFrontClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASightRear> SightRearClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AStock> StockClass;

	/*UPROPERTY(EditAnywhere)
	TSubclassOf<ATactical> TacticalClass;*/

};

UCLASS()
class SHOOTER_API UWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, category = "ModData")
	FModData ModData;

};
