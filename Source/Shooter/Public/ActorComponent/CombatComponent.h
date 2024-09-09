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
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
	void Server_WeaponIdle();

	UFUNCTION(Server, Reliable)
	void Server_WeaponIdleToOut();

	UFUNCTION(Server, Reliable)
	void Server_WeaponOut();

	UFUNCTION(Server, Reliable)
	void Server_WeaponOutToIdle();

	UFUNCTION(Server, Reliable)
	void Server_EquipWeapon(AWeapon* WeaponToEquip, USkeletalMeshComponent* ParentSkeletalMesh, FName InParentSocketName = NAME_None);

	UFUNCTION(Server, Reliable)
	void Server_UnequipWeapon(USkeletalMeshComponent* ParentSkeletalMesh, FName InParentSocketName);

	void SetIsAiming(bool bAiming);
	void ReloadWeapon();

	FOnCombatComponentWeaponAnimNotifySignature OnCombatComponentWeaponIdle;
	FOnCombatComponentWeaponAnimNotifySignature OnCombatComponentWeaponIdleToOut;
	FOnCombatComponentWeaponAnimNotifySignature OnCombatComponentWeaponOut;
	FOnCombatComponentWeaponAnimNotifySignature OnCombatComponentWeaponOutToIdle;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION(Server, Reliable)
	void Server_SetIsAiming(bool bAiming);

	void SetCombatAction(ECombatAction Action);

	UFUNCTION()
	void Handle_OnWeaponIdle(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnWeaponIdleToOut(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnWeaponOut(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnWeaponOutToIdle(AWeapon* Weapon);

	UFUNCTION()
	void OnRep_EquippedWeapon(AWeapon* PrevEquippedWeapon);

	UFUNCTION()
	void OnRep_CombatAction();

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<AWeapon> EquippedWeapon;

	UPROPERTY(Replicated)
	bool bIsAiming;

	UPROPERTY(ReplicatedUsing = OnRep_CombatAction)
	ECombatAction CombatAction;

public:
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE ECombatAction GetCombatAction() const { return CombatAction; }
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }
	FORCEINLINE bool HasAuthority() const { return GetOwner() && GetOwner()->HasAuthority(); }

};
