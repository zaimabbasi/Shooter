// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enum/CombatAction.h"
#include "Weapon.generated.h"

class AAmmo;
class AForegrip;
class AHandguard;
class AMag;
class AShooterCharacter;
class UBoxComponent;
class UHandsAnimationDataAsset;
class UModComponent;
class UModDataAsset;
class UWeaponAnimationDataAsset;
class UWeaponDataAsset;
enum class EWeaponFiremode : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAnimNotifySignature, AWeapon*, Weapon);

UCLASS()
class SHOOTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	bool DoesNeedCharge();
	
	template<class T>
	T* GetAttachedActor() const
	{
		T* AttachedMod = nullptr;
		TArray<AActor*> AttachedActors;
		GetAttachedActors(AttachedActors, false, true);
		AttachedActors.FindItemByClass(&AttachedMod);
		return AttachedMod;
	}
	
	ECombatAction GetCombatAction() const;
	EWeaponFiremode GetFiremode() const;
	AForegrip* GetForegrip() const;
	AHandguard* GetHandguard() const;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	bool GetIsOneHanded() const;
	uint8 GetMagAmmoCount() const;
	uint8 GetMagAmmoSpace() const;
	uint16 GetRateOfFire() const;
	bool HasFiremodes();
	bool HasMag();
	bool HasPatronInWeaponAmmo();
	virtual void Init();
	virtual bool IsPistol() const { return false; }
	virtual void PostInitializeComponents() override;

	UFUNCTION(Server, Reliable)
	void Server_MagAddAmmo(const uint8 Count);

	UFUNCTION(Server, Reliable)
	void Server_MagPopAmmo();

	UFUNCTION(Server, Reliable)
	void Server_SetIsHolster(const bool bHolster);

	UFUNCTION(Server, Reliable)
	void Server_SwitchFiremode();

	virtual void SetOwner(AActor* NewOwner) override;
	virtual void Tick(float DeltaTime) override;

	FOnWeaponAnimNotifySignature OnWeaponActionEnd;
	FOnWeaponAnimNotifySignature OnWeaponActionStart;
	FOnWeaponAnimNotifySignature OnWeaponChamberCheck;
	FOnWeaponAnimNotifySignature OnWeaponFire;
	FOnWeaponAnimNotifySignature OnWeaponFireDry;
	FOnWeaponAnimNotifySignature OnWeaponFiremode;
	FOnWeaponAnimNotifySignature OnWeaponFiremodeCheck;
	FOnWeaponAnimNotifySignature OnWeaponIdle;
	FOnWeaponAnimNotifySignature OnWeaponIdleToOut;
	FOnWeaponAnimNotifySignature OnWeaponMagCheck;
	FOnWeaponAnimNotifySignature OnWeaponMagIn;
	FOnWeaponAnimNotifySignature OnWeaponMagOut;
	FOnWeaponAnimNotifySignature OnWeaponOut;
	FOnWeaponAnimNotifySignature OnWeaponOutToIdle;
	FOnWeaponAnimNotifySignature OnWeaponOutToIdleArm;
	FOnWeaponAnimNotifySignature OnWeaponReloadCharge;

protected:
	virtual void BeginPlay() override;
	AMag* GetMag() const;

	TObjectPtr<AShooterCharacter> ShooterCharacterOwner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ActorComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UModComponent> ModComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataAsset", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UWeaponDataAsset> WeaponDataAsset;

	UPROPERTY(Replicated)
	bool bIsHolster;

	UPROPERTY(Replicated)
	uint8 FiremodeIndex;

	UPROPERTY(Replicated)
	TObjectPtr<AAmmo> PatronInWeaponAmmo;

	UPROPERTY(Replicated)
	TObjectPtr<AAmmo> ShellPortAmmo;

private:
	void EjectShellPortAmmo();

	UFUNCTION()
	void Handle_OnMagAmmoPopped(AAmmo* PoppedAmmo);

	UFUNCTION()
	void Handle_OnWeaponAnimInstanceActionEnd();

	UFUNCTION()
	void Handle_OnWeaponAnimInstanceActionStart();

	UFUNCTION()
	void Handle_OnWeaponAnimInstanceChamberCheck();

	UFUNCTION()
	void Handle_OnWeaponAnimInstanceFire();

	UFUNCTION()
	void Handle_OnWeaponAnimInstanceFireDry();

	UFUNCTION()
	void Handle_OnWeaponAnimInstanceFiremode();

	UFUNCTION()
	void Handle_OnWeaponAnimInstanceFiremodeCheck();

	UFUNCTION()
	void Handle_OnWeaponAnimInstanceIdle();

	UFUNCTION()
	void Handle_OnWeaponAnimInstanceIdleToOut();

	UFUNCTION()
	void Handle_OnWeaponAnimInstanceMagCheck();

	UFUNCTION()
	void Handle_OnWeaponAnimInstanceMagIn();

	UFUNCTION()
	void Handle_OnWeaponAnimInstanceMagOut();

	UFUNCTION()
	void Handle_OnWeaponAnimInstanceOut();

	UFUNCTION()
	void Handle_OnWeaponAnimInstanceOutToIdle();

	UFUNCTION()
	void Handle_OnWeaponAnimInstanceOutToIdleArm();

	UFUNCTION()
	void Handle_OnWeaponAnimInstancePatronInWeapon();

	UFUNCTION()
	void Handle_OnWeaponAnimInstanceReloadCharge();

	UFUNCTION()
	void Handle_OnWeaponAnimInstanceShellPort();

	UFUNCTION()
	void Handle_OnWeaponAnimInstanceWeaponHammer();

	UFUNCTION()
	void Handle_OnWeaponAnimInstanceWeaponSelector();

public:
	FORCEINLINE bool GetIsHolster() const { return bIsHolster; }
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }
	FORCEINLINE AShooterCharacter* GetShooterCharacterOwner() const { return ShooterCharacterOwner; }
	FORCEINLINE bool HasAuthority() const { return GetOwner() && GetOwner()->HasAuthority(); }

};
