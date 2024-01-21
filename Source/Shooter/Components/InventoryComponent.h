// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Shooter/Data/CharacterDataAsset.h"
#include "InventoryComponent.generated.h"

class AShooterCharacter;
class AWeapon;

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

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_SetCurrentIndex(uint8 Index);

private:
	AShooterCharacter* OwningCharacter;

	TArray<FInventoryData> InventoryDataArray;

	UPROPERTY(Replicated)
	TArray<AWeapon*> WeaponsArray;

	UPROPERTY(Replicated)
	uint8 CurrentIndex;

public:
	AWeapon* GetWeaponAtIndex(uint32 Index);

};
