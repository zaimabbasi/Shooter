// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterCombatComponent.generated.h"

class AWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatComponentWeaponAnimNotifySignature, AWeapon*, Weapon);

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
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ActionEnd();
	void ActionStart();
	void Idle();
	void IdleToOut();
	void Out();
	void OutToIdle();
	void WeaponChamberCheck();
	void WeaponFire(bool bFire);
	void WeaponFiremode();
	void WeaponFiremodeCheck();
	void WeaponMagCheck();
	void WeaponMagIn();
	void WeaponMagOut();
	void WeaponReloadCharge();

	void EquipWeapon(AWeapon* WeaponToEquip, USkeletalMeshComponent* ParentSkeletalMesh, FName InParentSocketName = NAME_None);
	void UnequipWeapon(USkeletalMeshComponent* ParentSkeletalMesh, FName InParentSocketName);

	FOnCombatComponentWeaponAnimNotifySignature OnCombatComponentWeaponActionEnd;
	FOnCombatComponentWeaponAnimNotifySignature OnCombatComponentWeaponActionStart;
	FOnCombatComponentWeaponAnimNotifySignature OnCombatComponentWeaponChamberCheck;
	FOnCombatComponentWeaponAnimNotifySignature OnCombatComponentWeaponFire;
	FOnCombatComponentWeaponAnimNotifySignature OnCombatComponentWeaponFireDry;
	FOnCombatComponentWeaponAnimNotifySignature OnCombatComponentWeaponFiremode;
	FOnCombatComponentWeaponAnimNotifySignature OnCombatComponentWeaponFiremodeCheck;
	FOnCombatComponentWeaponAnimNotifySignature OnCombatComponentWeaponIdle;
	FOnCombatComponentWeaponAnimNotifySignature OnCombatComponentWeaponIdleToOut;
	FOnCombatComponentWeaponAnimNotifySignature OnCombatComponentWeaponMagCheck;
	FOnCombatComponentWeaponAnimNotifySignature OnCombatComponentWeaponMagIn;
	FOnCombatComponentWeaponAnimNotifySignature OnCombatComponentWeaponMagOut;
	FOnCombatComponentWeaponAnimNotifySignature OnCombatComponentWeaponOut;
	FOnCombatComponentWeaponAnimNotifySignature OnCombatComponentWeaponOutToIdle;
	FOnCombatComponentWeaponAnimNotifySignature OnCombatComponentWeaponOutToIdleArm;
	FOnCombatComponentWeaponAnimNotifySignature OnCombatComponentWeaponReloadCharge;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void Handle_OnWeaponActionEnd(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnWeaponActionStart(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnWeaponChamberCheck(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnWeaponFire(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnWeaponFireDry(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnWeaponFiremode(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnWeaponFiremodeCheck(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnWeaponIdle(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnWeaponIdleToOut(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnWeaponMagCheck(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnWeaponMagIn(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnWeaponMagOut(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnWeaponOut(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnWeaponOutToIdle(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnWeaponOutToIdleArm(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnWeaponReloadCharge(AWeapon* Weapon);

	UFUNCTION()
	void OnRep_CombatAction();

	UFUNCTION()
	void OnRep_EquippedWeapon(AWeapon* PrevEquippedWeapon);

	/*UPROPERTY(Replicated)
	bool bIsFiring;*/

	UPROPERTY(ReplicatedUsing = OnRep_CombatAction)
	ECombatAction CombatAction;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<AWeapon> EquippedWeapon;

	bool bWantsToFire;

public:
	FORCEINLINE ECombatAction GetCombatAction() const { return CombatAction; }
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE bool HasAuthority() const { return GetOwner() && GetOwner()->HasAuthority(); }

};
