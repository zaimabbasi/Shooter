// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysicalMaterials/ShooterPhysicalMaterial.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"

void UShooterPhysicalMaterial::HandleBulletImpact_Implementation(const FHitResult& HitResult)
{
	UGameplayStatics::SpawnSoundAtLocation(HitResult.GetComponent(), BulletImpactSound.LoadSynchronous(), HitResult.Location);

	UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(HitResult.GetComponent(), BulletDecalMaterial.LoadSynchronous(), FVector(1.0f, 4.0f, 4.0f), HitResult.Location, HitResult.Normal.Rotation());	
	if (DecalComponent)
	{
		DecalComponent->SetFadeScreenSize(0.0f);
	}
}
