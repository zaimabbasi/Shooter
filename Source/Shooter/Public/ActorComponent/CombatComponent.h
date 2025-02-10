// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enum/CombatAction.h"
#include "CombatComponent.generated.h"

class AWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatComponentWeaponAnimNotifySignature, AWeapon*, Weapon);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();
	
	friend class AShooterCharacter;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(Server, Reliable)
	void Server_ActionEnd();

	UFUNCTION(Server, Reliable)
	void Server_ActionStart();

	UFUNCTION(Server, Reliable)
	void Server_EquipWeapon(AWeapon* WeaponToEquip, USkeletalMeshComponent* ParentSkeletalMesh, FName InParentSocketName = NAME_None);

	UFUNCTION(Server, Reliable)
	void Server_Idle();

	UFUNCTION(Server, Reliable)
	void Server_IdleToOut();

	UFUNCTION(Server, Reliable)
	void Server_Out();

	UFUNCTION(Server, Reliable)
	void Server_OutToIdle();

	UFUNCTION(Server, Reliable)
	void Server_UnequipWeapon(USkeletalMeshComponent* ParentSkeletalMesh, FName InParentSocketName);

	UFUNCTION(Server, Reliable)
	void Server_WeaponChamberCheck();

	UFUNCTION(Server, Reliable)
	void Server_WeaponFire(const bool bFiring);

	UFUNCTION(Server, Reliable)
	void Server_WeaponFiremode();

	UFUNCTION(Server, Reliable)
	void Server_WeaponFiremodeCheck();

	UFUNCTION(Server, Reliable)
	void Server_WeaponMagCheck();

	UFUNCTION(Server, Reliable)
	void Server_WeaponMagIn();

	UFUNCTION(Server, Reliable)
	void Server_WeaponMagOut();

	UFUNCTION(Server, Reliable)
	void Server_WeaponReloadCharge();

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

	UFUNCTION(Server, Reliable)
	void Server_SetCombatAction(ECombatAction Action);

	UFUNCTION(Server, Reliable)
	void Server_SetIsFiring(const bool bFiring);

	UPROPERTY(Replicated)
	bool bIsFiring;

	UPROPERTY(ReplicatedUsing = OnRep_CombatAction)
	ECombatAction CombatAction;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<AWeapon> EquippedWeapon;

public:
	FORCEINLINE ECombatAction GetCombatAction() const { return CombatAction; }
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE bool HasAuthority() const { return GetOwner() && GetOwner()->HasAuthority(); }

};
