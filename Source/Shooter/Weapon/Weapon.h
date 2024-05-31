// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class AShooterCharacter;
class UBoxComponent;
class UHandsAnimationDataAsset;
class UModComponent;
class UWeaponAnimationDataAsset;
class UWeaponDataAsset;

UCLASS()
class SHOOTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	void SetActorHiddenInGameWithChildren(bool bNewHidden);

protected:
	virtual void BeginPlay() override;

	AShooterCharacter* OwningCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataAsset", meta = (AllowPrivateAccess = "true"))
	UWeaponDataAsset* WeaponDataAsset;

	UModComponent* ModComponent;

public:
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }
	UClass* GetHandsAnimClass() const;
	USkeletalMeshComponent* GetAimCameraSocketParentMesh();
	FName GetAimCameraSocketName();
};
