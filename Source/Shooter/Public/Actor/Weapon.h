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
class UHandsAnimationDataAsset;
class UModComponent;
class UModDataAsset;
class UWeaponAnimationDataAsset;
class UWeaponDataAsset;

UENUM(BlueprintType)
enum class EWeaponFiremode : uint8
{
	WF_None UMETA(DisplayName = "None"),
	WF_SingleShot UMETA(DisplayName = "SingleShot"),
	WF_2RoundsBurst UMETA(DisplayName = "2RoundsBurst"),
	WF_3RoundsBurst UMETA(DisplayName = "3RoundsBurst"),
	WF_FullAuto UMETA(DisplayName = "FullAuto"),
	Default_MAX UMETA(Hidden)
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponAnimNotifySignature, AWeapon*, Weapon);

UCLASS()
class SHOOTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	AWeapon();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void SetOwner(AActor* NewOwner) override;
	virtual void Tick(float DeltaTime) override;

	virtual void Init();
	virtual bool IsPistol() const { return false; }
	
	bool GetIsOneHanded() const;
	uint16 GetRateOfFire() const;
	uint8 GetNumFiremodes() const;

	template<class T>
	T* GetAttachedActor() const
	{
		T* AttachedMod = nullptr;
		TArray<AActor*> AttachedActors;
		GetAttachedActors(AttachedActors, false, true);
		AttachedActors.FindItemByClass(&AttachedMod);
		return AttachedMod;
	}

	EWeaponFiremode GetFiremode() const;

	//AForegrip* GetForegrip() const;
	//AHandguard* GetHandguard() const;
	AMag* GetMag() const;

	USkeletalMeshComponent* GetForegripHandguardMesh() const;
	USkeletalMeshComponent* GetScopeSightMesh() const;

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

	UPROPERTY(Replicated)
	bool bIsHolster;

protected:
	virtual void BeginPlay() override;

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
	uint8 FiremodeIndex;

	UPROPERTY(Replicated)
	TObjectPtr<AAmmo> PatronInWeaponAmmo;

	UPROPERTY(Replicated)
	TObjectPtr<AAmmo> ShellPortAmmo;

private:
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
	FORCEINLINE AAmmo* GetPatronInWeaponAmmo() const { return PatronInWeaponAmmo; }
	FORCEINLINE AShooterCharacter* GetShooterCharacterOwner() const { return ShooterCharacterOwner; }
	FORCEINLINE bool HasAuthority() const { return GetOwner() && GetOwner()->HasAuthority(); }

};
