// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class AWeapon;
struct FInventoryParams;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryComponentWeaponArrayReplicatedSignature);

#define PRIMARY_WEAPON_INDEX	(0)
#define SECONDARY_WEAPON_INDEX	(1)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();
	int8 FindWeapon(AWeapon*& Weapon) const;
	uint8 GetAmmoAtIndex(uint8 Index);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	AWeapon* GetWeaponAtIndex(uint8 Index);
	void Init(const FInventoryParams& InventoryParams);
	
	UFUNCTION(Server, Reliable)
	void Server_LoadAmmoInWeaponMag(AWeapon* Weapon, const uint8 AmmoCount);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FOnInventoryComponentWeaponArrayReplicatedSignature OnInventoryComponentWeaponArrayReplicated;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnRep_WeaponArray() const;

	UPROPERTY(Replicated)
	TArray<uint8> WeaponAmmoArray;
	
	UPROPERTY(ReplicatedUsing = OnRep_WeaponArray)
	TArray<TObjectPtr<AWeapon>> WeaponArray;

public:
	FORCEINLINE TArray<AWeapon*> GetWeaponArray() const { return WeaponArray; }
	
};
