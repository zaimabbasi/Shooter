// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponDataAsset.generated.h"

class ABarrel;
class AFlashlight;
class AForegrip;
class AGasblock;
class AHandguard;
class AMagazine;
class AMount;
class AMuzzle;
class APistolgrip;
class AReciever;
class AScope;
class ASightFront;
class ASightRear;
class AStock;
class ATactical;

USTRUCT()
struct FAttachmentData
{
	GENERATED_BODY()

public:
	TSubclassOf<ABarrel> BarrelClass;
	TSubclassOf<AFlashlight> FlashlightClass;
	TSubclassOf<AForegrip> ForegripClass;
	TSubclassOf<AGasblock> GasblockClass;
	TSubclassOf<AHandguard> HandguardClass;
	TSubclassOf<AMagazine> MagazineClass;
	TSubclassOf<AMount> MountClass;
	TSubclassOf<AMuzzle> MuzzleClass;
	TSubclassOf<APistolgrip> PistolgripClass;
	TSubclassOf<AReciever> RecieverClass;
	TSubclassOf<AScope> ScopeClass;
	TSubclassOf<ASightFront> SightFrontClass;
	TSubclassOf<ASightRear> SightRearClass;
	TSubclassOf<AStock> StockClass;
	TSubclassOf<ATactical> TacticalClass;

};

UCLASS()
class SHOOTER_API UWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, category = "AttachmentData")
	FAttachmentData AttachmentData;

};
