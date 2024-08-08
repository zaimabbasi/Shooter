// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class AWeapon;
struct FInventoryParams;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRepWeaponArraySignature);

#define PRIMARY_WEAPON_INDEX	(0)
#define SECONDARY_WEAPON_INDEX	(1)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FOnRepWeaponArraySignature OnRepWeaponArrayDelegate;

protected:
	virtual void BeginPlay() override;

public:
	void Init(const FInventoryParams& InventoryParams);
	int8 FindWeapon(AWeapon*& Weapon) const;
	void LoadAmmoInWeaponMag(AWeapon* Weapon, const uint8 AmmoCount);
	AWeapon* GetWeaponAtIndex(uint8 Index);
	uint8 GetAmmoAtIndex(uint8 Index);

private:
	UFUNCTION(Server, Reliable)
	void Server_LoadAmmoInWeaponMag(AWeapon* Weapon, const uint8 AmmoCount);

	UFUNCTION()
	void OnRep_WeaponArray();

private:
	UPROPERTY(ReplicatedUsing = OnRep_WeaponArray)
	TArray<TObjectPtr<AWeapon>> WeaponArray;

	UPROPERTY(Replicated)
	TArray<uint8> WeaponAmmoArray;

public:
	FORCEINLINE TArray<AWeapon*> GetWeaponArray() const { return WeaponArray; }
	
};
