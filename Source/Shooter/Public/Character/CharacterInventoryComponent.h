// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterInventoryComponent.generated.h"

class AWeapon;
struct FInventoryParams;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryComponentWeaponArrayReplicatedSignature);

#define PRIMARY_WEAPON_INDEX	(0)
#define SECONDARY_WEAPON_INDEX	(1)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UCharacterInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCharacterInventoryComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Init(const FInventoryParams& InventoryParams);
	int8 FindWeapon(AWeapon*& Weapon) const;
	AWeapon* GetWeaponAtIndex(uint8 Index) const;
	uint8 GetWeaponAmmoAtIndex(uint8 Index) const;
	void LoadAmmoInWeaponMag(uint8 WeaponIndex);

	FOnInventoryComponentWeaponArrayReplicatedSignature OnInventoryComponentWeaponArrayReplicated;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnRep_WeaponArray() const;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponArray)
	TArray<TObjectPtr<AWeapon>> WeaponArray;

	UPROPERTY(Replicated)
	TArray<uint8> WeaponAmmoArray;

public:
	FORCEINLINE TArray<AWeapon*> GetWeaponArray() const { return WeaponArray; }
	
};
