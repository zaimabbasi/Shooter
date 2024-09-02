// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponDataAsset.generated.h"

class UModDataAsset;

UCLASS()
class SHOOTER_API UWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "DataAsset")
	TSoftObjectPtr<UModDataAsset> ModDataAsset;

	/*UPROPERTY(EditAnywhere, Category = "HandsAnimClass")
	TSubclassOf<UAnimInstance> HandsAnimClass;*/

};
