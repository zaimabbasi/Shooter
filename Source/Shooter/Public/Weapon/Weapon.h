// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class AAmmo;
//class AForegrip;
//class AHandguard;
class AMag;
class AShooterCharacter;
class UBoxComponent;
class UHandsAnimationDataAsset;
class UWeaponModComponent;
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

	bool IsPlayingProceduralIdle() const;
	bool IsPlayingProceduralWalk() const;

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

	UFUNCTION()
	void Handle_OnIdleAnimHorizontalMovementUpdate(float Value);

	UFUNCTION()
	void Handle_OnIdleAnimVerticalMovementUpdate(float Value);

	UFUNCTION()
	void Handle_OnIdleAnimRollRotationUpdate(float Value);

	UFUNCTION()
	void Handle_OnWalkAnimHorizontalMovementUpdate(float Value);

	UFUNCTION()
	void Handle_OnWalkAnimVerticalMovementUpdate(float Value);

	UFUNCTION()
	void Handle_OnWalkAnimRollRotationUpdate(float Value);

	TObjectPtr<AShooterCharacter> ShooterCharacterOwner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ActorComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWeaponModComponent> WeaponModComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ActorComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTimelineComponent> IdleAnimTimeline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ActorComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTimelineComponent> WalkAnimTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | Idle", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UCurveFloat> IdleAnimHorizontalMovementCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | Idle", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UCurveFloat> IdleAnimVerticalMovementCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | Idle", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UCurveFloat> IdleAnimRollRotationCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | Walk", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UCurveFloat> WalkAnimHorizontalMovementCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | Walk", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UCurveFloat> WalkAnimVerticalMovementCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation | Walk", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UCurveFloat> WalkAnimRollRotationCurve;

	FOnTimelineFloat OnIdleAnimHorizontalMovementUpdate;
	FOnTimelineFloat OnIdleAnimVerticalMovementUpdate;
	FOnTimelineFloat OnIdleAnimRollRotationUpdate;

	FOnTimelineFloat OnWalkAnimHorizontalMovementUpdate;
	FOnTimelineFloat OnWalkAnimVerticalMovementUpdate;
	FOnTimelineFloat OnWalkAnimRollRotationUpdate;

	float IdleAnimHorizontalMovementAlpha;
	float IdleAnimVerticalMovementAlpha;
	float IdleAnimRollRotationAlpha;
	
	float WalkAnimHorizontalMovementAlpha;
	float WalkAnimVerticalMovementAlpha;
	float WalkAnimRollRotationAlpha;

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

	UFUNCTION()
	void Handle_OnWeaponAnimInstancePlayProceduralIdle();
	
	UFUNCTION()
	void Handle_OnWeaponAnimInstancePlayProceduralWalk();

	UFUNCTION()
	void Handle_OnWeaponAnimInstanceStopProceduralAnim();

public:
	FORCEINLINE bool GetIsHolster() const { return bIsHolster; }
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }
	FORCEINLINE AAmmo* GetPatronInWeaponAmmo() const { return PatronInWeaponAmmo; }
	FORCEINLINE AShooterCharacter* GetShooterCharacterOwner() const { return ShooterCharacterOwner; }
	FORCEINLINE bool HasAuthority() const { return GetOwner() && GetOwner()->HasAuthority(); }
	FORCEINLINE float GetIdleAnimHorizontalMovementAlpha() const { return IdleAnimHorizontalMovementAlpha; }
	FORCEINLINE float GetIdleAnimVerticalMovementAlpha() const { return IdleAnimVerticalMovementAlpha; }
	FORCEINLINE float GetIdleAnimRollRotationAlpha() const { return IdleAnimRollRotationAlpha; }
	FORCEINLINE float GetWalkAnimHorizontalMovementAlpha() const { return WalkAnimHorizontalMovementAlpha; }
	FORCEINLINE float GetWalkAnimVerticalMovementAlpha() const { return WalkAnimVerticalMovementAlpha; }
	FORCEINLINE float GetWalkAnimRollRotationAlpha() const { return WalkAnimRollRotationAlpha; }

};
