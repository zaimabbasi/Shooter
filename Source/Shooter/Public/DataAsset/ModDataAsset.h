// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ModDataAsset.generated.h"

class ABarrel;
class ACharge;
//class AFlashlight;
//class AForegrip;
class AGasBlock;
class AHandguard;
class AMag;
//class AMount;
class AMuzzle;
class APistolgrip;
class AReceiver;
//class AScope;
class ASightFront;
class ASightRear;
class AStock;
//class ATactical;

UCLASS()
class SHOOTER_API UModDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABarrel> BarrelClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACharge> ChargeClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AGasBlock> GasBlockClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AHandguard> HandguardClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AMag> MagClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AMuzzle> MuzzleClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<APistolgrip> PistolgripClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AReceiver> ReceiverClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASightFront> SightFrontClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASightRear> SightRearClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AStock> StockClass;
	
};
