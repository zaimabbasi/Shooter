// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/Impactable.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "ShooterPhysicalMaterial.generated.h"

UCLASS()
class SHOOTER_API UShooterPhysicalMaterial : public UPhysicalMaterial, public IImpactable
{
	GENERATED_BODY()

public:
	virtual void HandleBulletImpact_Implementation(const FHitResult& HitResult) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UMaterialInterface> BulletDecalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<USoundBase> BulletImpactSound;
};
