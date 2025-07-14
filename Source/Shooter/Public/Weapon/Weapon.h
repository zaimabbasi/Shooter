// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class AAmmo;
//class AForegrip;
//class AHandguard;
class AMag;
class AShooterCharacter;
class UBoxComponent;
class UWeaponModComponent;
class UWeaponDataAsset;
enum class ECombatAction : uint8;
enum class EWeaponFiremode : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAnimNotifySignature, AWeapon*, Weapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWeaponRecoilGeneratedSignature, AWeapon*, Weapon, float, RecoilHorizontalKick, float, RecoilVerticalKick);

UCLASS()
class SHOOTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	AWeapon();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetOwner(AActor* NewOwner) override;
	virtual void PostInitializeComponents() override;
	//virtual void Tick(float DeltaTime) override;

	virtual void Init();
	virtual bool IsPistol() const { return false; }
	
	bool GetIsOneHanded() const;
	uint16 GetRateOfFire() const;
	uint8 GetNumFiremodes() const;

	/*template<class T>
	T* GetAttachedActor() const
	{
		T* AttachedMod = nullptr;
		TArray<AActor*> AttachedActors;
		GetAttachedActors(AttachedActors, false, true);
		AttachedActors.FindItemByClass(&AttachedMod);
		return AttachedMod;
	}*/

	EWeaponFiremode GetFiremode() const;

	//AForegrip* GetForegrip() const;
	//AHandguard* GetHandguard() const;

	AMag* GetMag() const;
	USkeletalMeshComponent* GetForegripHandguardMesh() const;
	USkeletalMeshComponent* GetScopeSightMesh() const;

	UAnimInstance* GetAnimInstance() const;
	ECombatAction GetWeaponAnimCombatAction() const;
	void SetWeaponAnimCombatAction(ECombatAction CombatAction) const;

	virtual void PlayAudio() const;
	virtual void StopAudio() const;
	virtual void TriggerFireSound() const;
	virtual void StopFireSound() const;
	virtual void TriggerFireDrySound() const;

protected:
	virtual void BeginPlay() override;

	virtual void SpawnShellPortAmmo(TSubclassOf<AAmmo> AmmoClass);
	virtual void EjectShellPortAmmo();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ProxySpawnShellPortAmmo(TSubclassOf<AAmmo> AmmoClass);
	virtual void Multicast_ProxySpawnShellPortAmmo_Implementation(TSubclassOf<AAmmo> AmmoClass);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ProxyEjectShellPortAmmo();
	virtual void Multicast_ProxyEjectShellPortAmmo_Implementation();

	virtual void GenerateRecoil();

	UFUNCTION()
	virtual void Handle_OnWeaponAnimInstanceActionEnd();

	UFUNCTION()
	virtual void Handle_OnWeaponAnimInstanceActionStart();

	UFUNCTION()
	virtual void Handle_OnWeaponAnimInstanceChamberCheck();

	UFUNCTION()
	virtual void Handle_OnWeaponAnimInstanceFire();

	UFUNCTION()
	virtual void Handle_OnWeaponAnimInstanceFireDry();

	UFUNCTION()
	virtual void Handle_OnWeaponAnimInstanceFiremode();

	UFUNCTION()
	virtual void Handle_OnWeaponAnimInstanceFiremodeCheck();

	UFUNCTION()
	virtual void Handle_OnWeaponAnimInstanceIdle();

	UFUNCTION()
	virtual void Handle_OnWeaponAnimInstanceIdleToOut();

	UFUNCTION()
	virtual void Handle_OnWeaponAnimInstanceMagCheck();

	UFUNCTION()
	virtual void Handle_OnWeaponAnimInstanceMagIn();

	UFUNCTION()
	virtual void Handle_OnWeaponAnimInstanceMagOut();

	UFUNCTION()
	virtual void Handle_OnWeaponAnimInstanceOut();

	UFUNCTION()
	virtual void Handle_OnWeaponAnimInstanceOutToIdle();

	UFUNCTION()
	virtual void Handle_OnWeaponAnimInstanceOutToIdleArm();

	UFUNCTION()
	virtual void Handle_OnWeaponAnimInstancePatronInWeapon();

	UFUNCTION()
	virtual void Handle_OnWeaponAnimInstanceReloadCharge();

	UFUNCTION()
	virtual void Handle_OnWeaponAnimInstanceShellPort();

	UFUNCTION()
	virtual void Handle_OnWeaponAnimInstanceWeaponHammer();

	UFUNCTION()
	virtual void Handle_OnWeaponAnimInstanceWeaponSelector();

	virtual void LineTrace();

public:
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
	FOnWeaponAnimNotifySignature OnWeaponWeaponHammer;

	FOnWeaponRecoilGeneratedSignature OnWeaponRecoilGenerated;

protected:
	TObjectPtr<AShooterCharacter> ShooterCharacterOwner;

	UPROPERTY(Replicated)
	uint8 FiremodeIndex;

	TObjectPtr<AAmmo> PatronInWeaponAmmo;
	TObjectPtr<AAmmo> ShellPortAmmo;

	bool bIsArmed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", UIMax = "5", ClampMin = "0", ClampMax = "5", Units = "cm"))
	float RecoilKickMaxDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", UIMin = "0", UIMax = "5", ClampMin = "0", ClampMax = "5", Units = "deg"))
	float RecoilKickMaxPitchRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UWeaponDataAsset> WeaponDataAsset;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAudioComponent> AudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWeaponModComponent> WeaponModComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> BoxComponent;

public:
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }
	FORCEINLINE AShooterCharacter* GetShooterCharacterOwner() const { return ShooterCharacterOwner; }
	FORCEINLINE AAmmo* GetPatronInWeaponAmmo() const { return PatronInWeaponAmmo; }
	FORCEINLINE float GetRecoilKickMaxDistance() const { return RecoilKickMaxDistance; }
	FORCEINLINE float GetRecoilKickMaxPitchRotation() const { return RecoilKickMaxPitchRotation; }

};
