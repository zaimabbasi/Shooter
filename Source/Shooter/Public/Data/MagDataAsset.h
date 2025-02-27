// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MagDataAsset.generated.h"

class AAmmo;

UCLASS()
class SHOOTER_API UMagDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AAmmo> AmmoClass;

	UPROPERTY(EditAnywhere)
	uint8 AmmoCapacity;

};
