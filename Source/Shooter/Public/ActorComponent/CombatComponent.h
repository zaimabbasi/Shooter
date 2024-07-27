// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRepEquippedWeaponDelegate, AWeapon*, EquippedWeapon);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FOnRepEquippedWeaponDelegate OnRepEquippedWeaponDelegate;

protected:
	virtual void BeginPlay() override;
	
public:
	void SetEquippedWeapon(AWeapon* WeaponToEquip);
	void SetIsAiming(bool bAiming);
	void ReloadWeapon();

private:
	UFUNCTION(Server, Reliable)
	void Server_SetEquippedWeapon(AWeapon* WeaponToEquip);

	UFUNCTION(Server, Reliable)
	void Server_SetIsAiming(bool bAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

private:
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<AWeapon> EquippedWeapon;

	UPROPERTY(Replicated)
	bool bIsAiming;

public:
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }

};
