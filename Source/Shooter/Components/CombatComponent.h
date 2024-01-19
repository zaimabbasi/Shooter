// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AShooterCharacter;
class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	friend class AShooterCharacter;

	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;
	//void SetEquippedWeapon1P(AWeapon* WeaponToEquip1P);

	UFUNCTION(Server, Reliable)
	void Server_SetEquippedWeapon(AWeapon* WeaponToEquip);

	UFUNCTION()
	void OnRep_EquippedWeapon();

private:
	AShooterCharacter* OwningCharacter;

	UPROPERTY(ReplicatedUsing = "OnRep_EquippedWeapon")
	AWeapon* EquippedWeapon;

	//AWeapon* EquippedWeapon1P;
	
};
