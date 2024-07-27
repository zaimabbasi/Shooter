// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enum/WeaponAction.h"
#include "Enum/WeaponActionState.h"
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
	
protected:
	virtual void BeginPlay() override;

private:

public:
	virtual void Init();
	virtual bool IsPistol() const { return false; }
	virtual bool HandleAnimNotify(const FAnimNotifyEvent& AnimNotifyEvent);
	uint8 GetMagAmmoSpace() const;
	void LoadAmmoInChamber();

protected:
	UFUNCTION()
	void Handle_OnAmmoRemoved(AAmmo* RemovedAmmo);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ActorComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UModComponent> ModComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataAsset", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UWeaponDataAsset> WeaponDataAsset;

	UPROPERTY(Replicated)
	EWeaponAction WeaponAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actor", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AAmmo> AmmoInChamber;

private:

public:
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }
	FORCEINLINE EWeaponAction GetWeaponAction() const { return WeaponAction; }
	UClass* GetHandsAnimClass() const;
	AMag* GetMag();

};
