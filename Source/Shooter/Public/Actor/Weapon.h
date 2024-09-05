// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enum/CombatAction.h"
#include "Weapon.generated.h"

class AAmmo;
class AMag;
class AShooterCharacter;
class UBoxComponent;
class UHandsAnimationDataAsset;
class UModComponent;
class UModDataAsset;
class UWeaponAnimationDataAsset;
class UWeaponDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAnimNotifySignature, AWeapon*, Weapon);

UCLASS()
class SHOOTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

	virtual void Init();
	virtual bool IsPistol() const { return false; }
	uint8 GetMagAmmoSpace() const;
	void LoadAmmoInChamber();
	void SetCombatAction(ECombatAction Action);
	void SetIsHolster(const bool bHolster);

	FOnWeaponAnimNotifySignature OnWeaponIdle;
	FOnWeaponAnimNotifySignature OnWeaponIdleToOut;
	FOnWeaponAnimNotifySignature OnWeaponOut;
	FOnWeaponAnimNotifySignature OnWeaponOutToIdle;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_SetCombatAction(ECombatAction Action);

	UFUNCTION(Server, Reliable)
	void Server_SetIsHolster(const bool bHolster);

	UFUNCTION()
	void Handle_OnMagAmmoRemoved(AAmmo* RemovedAmmo);

	UFUNCTION()
	void Handle_OnWeaponAnimInstanceIdle();

	UFUNCTION()
	void Handle_OnWeaponAnimInstanceIdleToOut();

	UFUNCTION()
	void Handle_OnWeaponAnimInstanceOut();

	UFUNCTION()
	void Handle_OnWeaponAnimInstanceOutToIdle();

	FORCEINLINE FName GetPatronInWeaponSocketName() const { return TEXT("patron_in_weapon"); }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ActorComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UModComponent> ModComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataAsset", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UWeaponDataAsset> WeaponDataAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actor", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AAmmo> AmmoInChamber;

	UPROPERTY(Replicated)
	ECombatAction CombatAction;

	UPROPERTY(Replicated)
	bool bIsHolster;

public:
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }
	FORCEINLINE ECombatAction GetCombatAction() const { return CombatAction; }
	FORCEINLINE bool GetIsHolster() const { return bIsHolster; }
	FORCEINLINE bool HasAuthority() const { return GetOwner() && GetOwner()->HasAuthority(); }
	AMag* GetMag();

};
