// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterDataAsset.generated.h"

class AWeapon;
class UInventoryDataAsset;

UCLASS()
class SHOOTER_API UCharacterDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "DataAsset")
	TSoftObjectPtr<UInventoryDataAsset> InventoryDataAsset;

	UPROPERTY(EditAnywhere, Category = "HandsAnimClass")
	TSubclassOf<UAnimInstance> HandsAnimClass;

};
