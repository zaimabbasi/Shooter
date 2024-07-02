// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Shooter/Data/CharacterDataAsset.h"
#include "InventoryComponent.generated.h"

class AShooterCharacter;
class AWeapon;
class UInventoryDataAsset;

DECLARE_DELEGATE(FOnWeaponsArrayReadyDelegate)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	friend class AShooterCharacter;

	UInventoryComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	const uint8 PRIMARY_WEAPON_INDEX = 0;
	const uint8 SECONDARY_WEAPON_INDEX = 1;

	FOnWeaponsArrayReadyDelegate OnWeaponsArrayReadyDelegate;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_AddAmmoInWeaponMag(uint8 AmmoCount, uint8 WeaponIndex);

	UFUNCTION()
	void OnRep_WeaponsArray() const;

private:
	AShooterCharacter* OwningCharacter;

	UInventoryDataAsset* InventoryDataAsset;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponsArray)
	TArray<AWeapon*> WeaponsArray;

	UPROPERTY(Replicated)
	TArray<uint8> WeaponsAmmoArray;

public:
	AWeapon* GetWeaponAtIndex(uint8 Index);
	uint8 GetAmmoAtIndex(uint8 Index);

};
