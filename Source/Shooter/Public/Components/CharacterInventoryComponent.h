// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterInventoryComponent.generated.h"

class AWeapon;
struct FInventoryParams;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterInventoryWeaponArrayReplicatedSignature);

#define PRIMARY_WEAPON_INDEX	(0)
#define SECONDARY_WEAPON_INDEX	(1)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UCharacterInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCharacterInventoryComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void Init(const FInventoryParams& InventoryParams);
	virtual int8 FindWeapon(AWeapon* const& Weapon) const;
	virtual AWeapon* GetWeaponAtIndex(uint8 Index) const;
	virtual uint8 GetWeaponAmmoAtIndex(uint8 Index) const;
	virtual void LoadAmmoInWeaponMag(uint8 WeaponIndex);
	virtual void LoadAmmoInWeaponMag(uint8 WeaponIndex, uint8 AmmoCount);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnRep_WeaponArray() const;

public:
	FOnCharacterInventoryWeaponArrayReplicatedSignature OnCharacterInventoryWeaponArrayReplicated;

private:
	UPROPERTY(ReplicatedUsing = OnRep_WeaponArray)
	TArray<TObjectPtr<AWeapon>> WeaponArray;

	UPROPERTY(Replicated)
	TArray<uint8> WeaponAmmoArray;

public:
	FORCEINLINE TArray<AWeapon*> GetWeaponArray() const { return WeaponArray; }
	
};
