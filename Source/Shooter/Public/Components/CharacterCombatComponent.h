// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterCombatComponent.generated.h"

class AShooterCharacter;
class AWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterCombatEquippedWeaponChangedSignature, AWeapon*, Weapon);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UCharacterCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCharacterCombatComponent();
	friend class AShooterCharacter;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual bool CanEquipWeapon(AWeapon* WeaponToEquip);
	virtual void EquipWeapon(AWeapon* WeaponToEquip);

	virtual bool CanReloadWeapon();
	virtual void ReloadWeapon();

	virtual bool CanCheckWeaponChamber();
	virtual void CheckWeaponChamber();

	virtual bool CanFireWeapon();
	virtual void FireWeapon(bool bFire);

	virtual bool CanChangeWeaponFiremode();
	virtual void ChangeWeaponFiremode();

	virtual bool CanCheckWeaponFiremode();
	virtual void CheckWeaponFiremode();

	virtual bool CanCheckWeaponMag();
	virtual void CheckWeaponMag();

	UFUNCTION(Server, Reliable)
	void Server_EquipWeapon(AWeapon* WeaponToEquip);
	virtual void Server_EquipWeapon_Implementation(AWeapon* WeaponToEquip);

	UFUNCTION(Server, Reliable)
	void Server_ReloadWeapon();
	virtual void Server_ReloadWeapon_Implementation();

	UFUNCTION(Server, Reliable)
	void Server_CheckWeaponChamber();
	virtual void Server_CheckWeaponChamber_Implementation();

	UFUNCTION(Server, Reliable)
	void Server_FireWeapon(bool bFire);
	virtual void Server_FireWeapon_Implementation(bool bFire);

	UFUNCTION(Server, Reliable)
	void Server_ChangeWeaponFiremode();
	virtual void Server_ChangeWeaponFiremode_Implementation();

	UFUNCTION(Server, Reliable)
	void Server_CheckWeaponFiremode();
	virtual void Server_CheckWeaponFiremode_Implementation();

	UFUNCTION(Server, Reliable)
	void Server_CheckWeaponMag();
	virtual void Server_CheckWeaponMag_Implementation();

	ECombatAction GetWeaponAnimCombatAction() const;
	ECombatAction GetHandsAnimCombatAction() const;
	ECombatAction GetAnimCombatAction() const;

protected:
	virtual void BeginPlay() override;

	//virtual void ActionEnd();
	//virtual void ActionStart();
	//virtual void ChamberCheck();
	virtual void Fire();
	//virtual void FireDry();
	//virtual void Firemode();
	//virtual void FiremodeCheck();
	//virtual void Idle();
	//virtual void IdleToOut();
	//virtual void MagCheck();
	//virtual void MagIn();
	//virtual void MagOut();
	//virtual void Out();
	virtual void OutToIdle();
	//virtual void WeaponMagIn();
	//virtual void WeaponMagOut();
	//virtual void WeaponReloadCharge();
	
	virtual void AddDelegates(AWeapon* Weapon);
	virtual void RemoveDelegates(AWeapon* Weapon);

	UAnimInstance* GetRelevantAnimInstance() const;

	UFUNCTION()
	virtual void Handle_OnCharacterHandsAnimInstanceIdle();

	UFUNCTION()
	virtual void Handle_OnCharacterHandsAnimInstanceIdleToOut();

	UFUNCTION()
	virtual void Handle_OnCharacterHandsAnimInstanceOut();

	UFUNCTION()
	virtual void Handle_OnCharacterHandsAnimInstanceOutToIdle();

	UFUNCTION()
	virtual void Handle_OnWeaponActionEnd(AWeapon* Weapon);

	UFUNCTION()
	virtual void Handle_OnWeaponActionStart(AWeapon* Weapon);

	UFUNCTION()
	virtual void Handle_OnWeaponChamberCheck(AWeapon* Weapon);

	UFUNCTION()
	virtual void Handle_OnWeaponFire(AWeapon* Weapon);

	UFUNCTION()
	virtual void Handle_OnWeaponFireDry(AWeapon* Weapon);

	UFUNCTION()
	virtual void Handle_OnWeaponFiremode(AWeapon* Weapon);

	UFUNCTION()
	virtual void Handle_OnWeaponFiremodeCheck(AWeapon* Weapon);

	UFUNCTION()
	virtual void Handle_OnWeaponIdle(AWeapon* Weapon);

	UFUNCTION()
	virtual void Handle_OnWeaponIdleToOut(AWeapon* Weapon);

	UFUNCTION()
	virtual void Handle_OnWeaponMagCheck(AWeapon* Weapon);

	UFUNCTION()
	virtual void Handle_OnWeaponMagIn(AWeapon* Weapon);

	UFUNCTION()
	virtual void Handle_OnWeaponMagOut(AWeapon* Weapon);

	UFUNCTION()
	virtual void Handle_OnWeaponOut(AWeapon* Weapon);

	UFUNCTION()
	virtual void Handle_OnWeaponOutToIdle(AWeapon* Weapon);

	UFUNCTION()
	virtual void Handle_OnWeaponOutToIdleArm(AWeapon* Weapon);

	UFUNCTION()
	virtual void Handle_OnWeaponReloadCharge(AWeapon* Weapon);

	UFUNCTION()
	virtual void OnRep_CombatAction(ECombatAction PrevCombatAction);

	UFUNCTION()
	virtual void OnRep_EquippedWeapon(AWeapon* PrevEquippedWeapon);

public:
	FOnCharacterCombatEquippedWeaponChangedSignature OnCharacterCombatEquippedWeaponChanged;

protected:
	bool bWantsToFire;
	uint8 NumRoundsFired;

	TObjectPtr<AWeapon> NextWeapon;

private:
	TObjectPtr<AShooterCharacter> ShooterCharacter;

	UPROPERTY(ReplicatedUsing = OnRep_CombatAction)
	ECombatAction CombatAction;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<AWeapon> EquippedWeapon;

public:
	FORCEINLINE ECombatAction GetCombatAction() const { return CombatAction; }
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }

};
