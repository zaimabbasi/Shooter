// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterCombatComponent.generated.h"

class AShooterCharacter;
class AWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatComponentEquippedWeaponChangedSignature, AWeapon*, Weapon);

UENUM(BlueprintType)
enum class ECombatAction : uint8
{
	CA_None UMETA(DisplayName = "None"),
	CA_ActionEnd UMETA(DisplayName = "ActionEnd"),
	CA_ActionStart UMETA(DisplayName = "ActionStart"),
	CA_ChamberCheck UMETA(DisplayName = "ChamberCheck"),
	CA_Fire UMETA(DisplayName = "Fire"),
	CA_Firemode UMETA(DisplayName = "Firemode"),
	CA_FiremodeCheck UMETA(DisplayName = "FiremodeCheck"),
	CA_Idle UMETA(DisplayName = "Idle"),
	CA_IdleToOut UMETA(DisplayName = "IdleToOut"),
	CA_MagCheck UMETA(DisplayName = "MagCheck"),
	CA_MagIn UMETA(DisplayName = "MagIn"),
	CA_MagOut UMETA(DisplayName = "MagOut"),
	CA_Out UMETA(DisplayName = "Out"),
	CA_OutToIdle UMETA(DisplayName = "OutToIdle"),
	CA_OutToIdleArm UMETA(DisplayName = "OutToIdleArm"),
	CA_ReloadCharge UMETA(DisplayName = "ReloadCharge"),
	Default_MAX UMETA(Hidden)
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UCharacterCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCharacterCombatComponent();
	friend class AShooterCharacter;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void EquipWeapon(AWeapon* WeaponToEquip);
	virtual void ReloadWeapon();

protected:
	virtual void BeginPlay() override;

	virtual void ActionEnd();
	virtual void ActionStart();
	virtual void Idle();
	virtual void IdleToOut();
	virtual void Out();
	virtual void OutToIdle();
	virtual void WeaponChamberCheck();
	virtual void WeaponFire(bool bFire);
	virtual void WeaponFiremode();
	virtual void WeaponFiremodeCheck();
	virtual void WeaponMagCheck();
	virtual void WeaponMagIn();
	virtual void WeaponMagOut();
	virtual void WeaponReloadCharge();

	virtual void EquipNextWeapon();
	virtual void UnequipWeapon();

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
	virtual void OnRep_CombatAction();

	UFUNCTION()
	virtual void OnRep_EquippedWeapon(AWeapon* PrevEquippedWeapon);

public:
	FOnCombatComponentEquippedWeaponChangedSignature OnCombatComponentEquippedWeaponChanged;

protected:
	/*UPROPERTY(Replicated)
	bool bIsFiring;*/

	bool bWantsToFire;

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
	FORCEINLINE bool HasAuthority() const { return GetOwner() && GetOwner()->HasAuthority(); }

};
