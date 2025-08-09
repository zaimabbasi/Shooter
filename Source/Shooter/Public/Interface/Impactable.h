// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Impactable.generated.h"

UINTERFACE(MinimalAPI)
class UImpactable : public UInterface
{
	GENERATED_BODY()
};

class SHOOTER_API IImpactable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void HandleBulletImpact(const FHitResult& HitResult);

};
