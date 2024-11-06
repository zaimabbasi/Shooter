// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SkeletalMergingLibrary.h"
#include "ShooterCharacter.generated.h"

class AWeapon;
class UCameraComponent;
class UCharacterDataAsset;
class UCombatComponent;
class UInputAction;
class UInputMappingContext;
class UInventoryComponent;
enum class ECharacterStance : uint8;
enum class ECombatAction : uint8;
enum class ELeanDirection : uint8;
enum class ETurnDirection : uint8;
struct FInputActionValue;

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AShooterCharacter();
	float GetAO_Pitch(float CurrentPitch, float DeltaTime) const;
	float GetAO_Yaw(float CurrentYaw, float DeltaTime);
	FName GetCharacterWeaponHolsterSocketName(AWeapon* Weapon) const;
	ECombatAction GetCombatAction() const;
	AWeapon* GetEquippedWeapon() const;
	bool GetIsAiming() const;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	ETurnDirection GetTurnDirection(float CurrentYaw);
	bool GetUseControllerDesiredRotation() const;
	float GetVelocityYawOffset() const;
	float GetYawExceedingMaxLimit(float CurrentYaw) const;
	bool HasVelocity() const;
	void Init();
	bool IsAccelerating() const;
	virtual void PostInitializeComponents() override;
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	bool CanPerformCrouchAction() const;
	bool CanPerformMoveAction() const;
	bool CanPerformProneAction() const;
	float GetMaxWalkSpeed() const;
	float GetMaxWalkSpeedCrouched() const;
	float GetMaxWalkSpeedCrouchedSlow() const;
	float GetMaxWalkSpeedProned() const;
	float GetMaxWalkSpeedSlow() const;
	float GetMaxWalkSpeedSprint() const;

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceTurnInPlace();

	UFUNCTION()
	void Handle_OnCharacterAnimInstanceControllerDesiredRotationNeeded(bool bControllerDesiredRotationNeeded);

	UFUNCTION()
	void Handle_OnCombatComponentActionEnd(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentActionStart(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentIdle(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentIdleToOut(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentOut(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentOutToIdle(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentOutToIdleArm(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentWeaponChamberCheck(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentWeaponFire(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentWeaponFireDry(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentWeaponFiremode(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentWeaponFiremodeCheck(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentWeaponMagCheck(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentWeaponMagIn(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentWeaponMagOut(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnCombatComponentWeaponReloadCharge(AWeapon* Weapon);

	UFUNCTION()
	void Handle_OnHandsAnimInstanceIdle();

	UFUNCTION()
	void Handle_OnHandsAnimInstanceIdleToOut();

	UFUNCTION()
	void Handle_OnHandsAnimInstanceOut();

	UFUNCTION()
	void Handle_OnHandsAnimInstanceOutToIdle();

	UFUNCTION()
	void Handle_OnInventoryComponentWeaponArrayReplicated();

	void OnCharacterAimAction(const FInputActionValue& Value);
	void OnCharacterAlterAction(const FInputActionValue& Value);
	void OnCharacterCrouchAction(const FInputActionValue& Value);
	void OnCharacterEquipPrimaryWeaponAction(const FInputActionValue& Value);
	void OnCharacterEquipSecondaryWeaponAction(const FInputActionValue& Value);
	void OnCharacterHolsterWeaponAction(const FInputActionValue& Value);
	void OnCharacterLeanLeftAction(const FInputActionValue& Value);
	void OnCharacterLeanRightAction(const FInputActionValue& Value);
	void OnCharacterLookAction(const FInputActionValue& Value);
	void OnCharacterMoveBackwardAction(const FInputActionValue& Value);
	void OnCharacterMoveForwardAction(const FInputActionValue& Value);
	void OnCharacterMoveLeftAction(const FInputActionValue& Value);
	void OnCharacterMoveRightAction(const FInputActionValue& Value);
	void OnCharacterProneAction(const FInputActionValue& Value);
	void OnCharacterSlowAction(const FInputActionValue& Value);
	void OnCharacterSprintAction(const FInputActionValue& Value);
	void OnWeaponChamberCheckAction(const FInputActionValue& Value);
	void OnWeaponFireAction(const FInputActionValue& Value);
	void OnWeaponFiremodeAction(const FInputActionValue& Value);
	void OnWeaponReloadAction(const FInputActionValue& Value);

	UFUNCTION()
	void OnRep_CurrentStance();

	UFUNCTION()
	void OnRep_IsToggleSlow();

	UFUNCTION()
	void OnRep_IsToggleSprint();

	UFUNCTION(Server, Reliable)
	void Server_EquipWeaponProgressive(AWeapon* WeaponToEquip);

	UFUNCTION(Server, Reliable)
	void Server_HolsterWeaponProgressive();

	UFUNCTION(Server, Reliable)
	void Server_SetCurrentStance(ECharacterStance NewStance);

	UFUNCTION(Server, Reliable)
	void Server_SetIsAlterAction(bool bAlterAction);

	UFUNCTION(Server, Reliable)
	void Server_SetIsToggleSlow(bool bToggleSlow);

	UFUNCTION(Server, Reliable)
	void Server_SetIsToggleSprint(bool bToggleSprint);

	UFUNCTION(Server, Reliable)
	void Server_SetLeanDirection(ELeanDirection NewLeanDirection);

	UFUNCTION(Server, Reliable)
	void Server_SetLeanTransitionDuration(float NewLeanTransitionDuration);

	UFUNCTION(Server, Reliable)
	void Server_SetLeaningRate(float Rate);

	void SetIsRemoteAccelerating(bool bRemoteAccelerating);
	void SetRemoteViewYaw(float NewRemoteYaw);
	void TransitionToSprint();
	void UpdateCameraFOV(float DeltaTime);
	void UpdateMaxWalkSpeed(ECharacterStance Stance, bool bToggleSlow, bool bToggleSprint);
	//void UpdateMovement(float DeltaTime);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> HandsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> LegsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ActorComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCombatComponent> CombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ActorComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataAsset", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UCharacterDataAsset> CharacterDataAsset;

	UPROPERTY(EditAnywhere, Category = "SkeletalMeshMergeParams")
	FSkeletalMeshMergeParams CharacterMeshMergeParams;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputMapping", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputMappingContext> CombatMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputMapping", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputMappingContext> InventoryMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputMapping", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputMappingContext> MovementMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputMapping", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputMappingContext> OtherMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterAimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterAlterAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterCrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterEquipPrimaryWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterEquipSecondaryWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterHolsterWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterLeanLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterLeanRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterMoveBackwardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterMoveForwardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterMoveLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterMoveRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterProneAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterSlowAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> CharacterSprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> WeaponChamberCheckAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> WeaponFireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> WeaponFiremodeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> WeaponReloadAction;

	float AimCameraFOV;

	UPROPERTY(Replicated)
	bool bIsAlterAction;

	bool bIsMoveInputBackward;
	bool bIsMoveInputForward;
	bool bIsMoveInputLeft;
	bool bIsMoveInputRight;

	UPROPERTY(Replicated)
	bool bIsRemoteAccelerating;

	UPROPERTY(ReplicatedUsing = OnRep_IsToggleSlow)
	bool bIsToggleSlow;

	UPROPERTY(ReplicatedUsing = OnRep_IsToggleSprint)
	bool bIsToggleSprint;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentStance)
	ECharacterStance CurrentStance;

	const float DefaultAnimationTransitionDuration = 0.25f;
	float DefaultCameraFOV;
	const float DefaultToAimCameraFOVPercentage = 0.8f;

	UPROPERTY(Replicated)
	ELeanDirection LeanDirection;

	UPROPERTY(Replicated)
	float LeaningRate;

	UPROPERTY(Replicated)
	float LeanTransitionDuration;

	const float MaxLean = 15.0f;

	TObjectPtr<AWeapon> NextWeaponToEquip;

	UPROPERTY(Replicated)
	uint8 RemoteViewYaw;

	UPROPERTY(Replicated)
	ETurnDirection TurnDirection;

public:
	FORCEINLINE ECharacterStance GetCurrentStance() const { return CurrentStance; }
	FORCEINLINE float GetDefaultAnimationTransitionDuration() const { return DefaultAnimationTransitionDuration; }
	FORCEINLINE float GetDefaultToAimCameraFOVPercentage() const { return DefaultToAimCameraFOVPercentage; }
	FORCEINLINE USkeletalMeshComponent* GetHandsMesh() const { return HandsMesh; }
	FORCEINLINE bool GetIsToggleSlow() const { return bIsToggleSlow; }
	FORCEINLINE bool GetIsToggleSprint() const { return bIsToggleSprint; }
	FORCEINLINE ELeanDirection GetLeanDirection() const { return LeanDirection; }
	FORCEINLINE float GetLeaningRate() const { return LeaningRate; }
	FORCEINLINE float GetLeanTransitionDuration() const { return LeanTransitionDuration; }
	FORCEINLINE float GetMaxLean() const { return MaxLean; }

};
