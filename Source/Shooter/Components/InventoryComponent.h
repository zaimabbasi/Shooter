// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Shooter/Data/CharacterDataAsset.h"
#include "InventoryComponent.generated.h"

class AShooterCharacter;
class AWeapon;

DECLARE_DELEGATE(FOnRepWeaponsArrayDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	friend class AShooterCharacter;

	UInventoryComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FOnRepWeaponsArrayDelegate OnRepWeaponsArrayDelegate;
	//FOnRepWeaponsArrayDelegate OnRepWeaponsArrayDelegate1P;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_WeaponsArray();

	/*UFUNCTION()
	void OnRep_WeaponsArray1P();*/

private:
	AShooterCharacter* OwningCharacter;

	TArray<FInventoryData> InventoryDataArray;

	UPROPERTY(ReplicatedUsing = "OnRep_WeaponsArray")
	TArray<AWeapon*> WeaponsArray;

	/*UPROPERTY(ReplicatedUsing = "OnRep_WeaponsArray1P")
	TArray<AWeapon*> WeaponsArray1P;*/

public:
	AWeapon* GetWeaponAtIndex(uint32 index);
	//AWeapon* GetWeaponAtIndex1P(uint32 index);
};
