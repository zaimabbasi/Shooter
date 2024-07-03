// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shooter/Types/WeaponAction.h"
#include "Shooter/Types/WeaponActionState.h"
#include "Weapon.generated.h"

class AAmmo;
class AMag;
class AShooterCharacter;
class UBoxComponent;
class UHandsAnimationDataAsset;
class UModComponent;
class UModDataAsset;
class UWeaponAnimationDataAsset;
class UWeaponDataAsset;

UCLASS()
class SHOOTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual bool HandleAnimNotify(const FAnimNotifyEvent& AnimNotifyEvent);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataAsset", meta = (AllowPrivateAccess = "true"))
	UWeaponDataAsset* WeaponDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataAsset", meta = (AllowPrivateAccess = "true"))
	UModDataAsset* ModDataAsset;

	UModComponent* ModComponent;

	UPROPERTY(Replicated)
	EWeaponAction WeaponAction;

	AAmmo* AmmoInChamber;

public:
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }
	FORCEINLINE EWeaponAction GetWeaponAction() const { return WeaponAction; }
	UClass* GetHandsAnimClass() const;
	AMag* GetMag();

};
