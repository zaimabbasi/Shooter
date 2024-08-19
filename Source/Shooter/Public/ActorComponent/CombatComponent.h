// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatComponentWeaponAnimNotifySignature, AWeapon*, Weapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCombatComponentEquippedWeaponReplicatedSignature, AWeapon*, EquippedWeapon, AWeapon*, PrevEquippedWeapon);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetEquippedWeapon(AWeapon* WeaponToEquip);
	void WeaponAttach(USkeletalMeshComponent* ParentSkeletalMesh, FName ParentSocketName);
	void SetWeaponDelegateBindings(AWeapon* Weapon);
	void ClearWeaponDelegateBindings(AWeapon* Weapon);
	void WeaponIdleToOut();
	void WeaponOutToIdle();
	void SetIsAiming(bool bAiming);
	void ReloadWeapon();

	FOnCombatComponentWeaponAnimNotifySignature OnCombatComponentWeaponIdle;
	FOnCombatComponentWeaponAnimNotifySignature OnCombatComponentWeaponIdleToOut;
	FOnCombatComponentWeaponAnimNotifySignature OnCombatComponentWeaponOut;
	FOnCombatComponentWeaponAnimNotifySignature OnCombatComponentWeaponOutToIdle;

	FOnCombatComponentEquippedWeaponReplicatedSignature OnCombatComponentEquippedWeaponReplicated;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION(Server, Reliable)
	void Server_SetEquippedWeapon(AWeapon* WeaponToEquip);

	UFUNCTION(Server, Reliable)
	void Server_WeaponAttach(USkeletalMeshComponent* ParentSkeletalMesh, FName ParentSocketName);

	UFUNCTION(Server, Reliable)
	void Server_SetIsAiming(bool bAiming);

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

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<AWeapon> EquippedWeapon;

	UPROPERTY(Replicated)
	bool bIsAiming;

public:
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE bool GetIsAiming() const { return bIsAiming; }
	FORCEINLINE bool HasAuthority() const { return GetOwner() && GetOwner()->HasAuthority(); }

};
