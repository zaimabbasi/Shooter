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
enum class ELeanDirection : uint8;
enum class ETurnDirection : uint8;
struct FInputActionValue;

UCLASS()
class SHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AShooterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;

public:
	void Init();
	virtual bool HandleHandsAnimNotify(const FAnimNotifyEvent& AnimNotifyEvent);
	AWeapon* GetEquippedWeapon();
	bool GetIsAiming();

private:
	void OnLookAction(const FInputActionValue& Value);
	void OnMoveForwardAction(const FInputActionValue& Value);
	void OnMoveBackwardAction(const FInputActionValue& Value);
	void OnMoveLeftAction(const FInputActionValue& Value);
	void OnMoveRightAction(const FInputActionValue& Value);
	void OnEquipPrimaryWeaponAction(const FInputActionValue& Value);
	void OnEquipSecondaryWeaponAction(const FInputActionValue& Value);
	void OnToggleCrouchAction(const FInputActionValue& Value);
	void OnToggleProneAction(const FInputActionValue& Value);
	void OnToggleSlowAction(const FInputActionValue& Value);
	void OnToggleSprintAction(const FInputActionValue& Value);
	void OnToggleLeanLeftAction(const FInputActionValue& Value);
	void OnToggleLeanRightAction(const FInputActionValue& Value);
	void OnToggleAimAction(const FInputActionValue& Value);
	void OnReloadWeaponAction(const FInputActionValue& Value);
	void UpdateMovement(float DeltaTime);
	void UpdateAO_Pitch(float DeltaTime);
	void UpdateCameraFOV(float DeltaTime);
	void TransitionToSprint();
	void SetMovementInputVector(float MovementInputX, float MovementInputY);
	/*void SetTurnDirection(ETurnDirection NewTurnDirection);*/
	void SetIsToggleSlow(bool bToggleSlow);
	void SetIsToggleSprint(bool bToggleSprint);
	void SetLeanDirection(ELeanDirection NewLeanDirection);
	void SetLeanTransitionDuration(float NewLeanTransitionDuration);
	void SetLeaningRate(float NewLeaningRate);
	void SetCurrentStance(ECharacterStance NewStance);
	FName GetCharacterWeaponHolsterSocketName(AWeapon* Weapon) const;
	FORCEINLINE FName GetHandsWeaponRootSocketName() const { return TEXT("weapon_rootSocket"); }

	UFUNCTION()
	void Handle_OnRepWeaponArray();

	UFUNCTION(Server, Reliable)
	void Server_EquipWeapon(AWeapon* WeaponToEquip);

	UFUNCTION(Server, Reliable)
	void Server_SetRemoteViewYaw(float RemoteYaw);

	UFUNCTION(Server, Reliable)
	void Server_SetMovementInputVector(float MovementInputX, float MovementInputY);

	UFUNCTION(Server, Reliable)
	void Server_SetTurnDirection(ETurnDirection NewTurnDirection);

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

	UFUNCTION(Server, Reliable)
	void Server_SetCurrentStance(ECharacterStance NewStance);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> LegsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> HandsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(EditAnywhere, Category = "SkeletalMeshMergeParams")
	FSkeletalMeshMergeParams CharacterMeshMergeParams;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ActorComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ActorComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCombatComponent> CombatComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputMapping", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputMappingContext> MovementMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputMapping", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputMappingContext> InventoryMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputMapping", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputMappingContext> CombatMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> MoveForwardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> MoveBackwardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> MoveLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> MoveRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> EquipPrimaryWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> EquipSecondaryWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> ToggleCrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> ToggleProneAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> ToggleSlowAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> ToggleSprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> ToggleLeanLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> ToggleLeanRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> ToggleAimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UInputAction> ReloadWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataAsset", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UCharacterDataAsset> CharacterDataAsset;

	TSubclassOf<UAnimInstance> DefaultHandsAnimClass;

	const float DefaultAnimationTransitionDuration = 0.25f;
	const float MaxLean = 15.0f;
	const float DefaultToAimCameraFOVPercentage = 0.8f;

	float AO_Yaw;
	float AO_Pitch;

	UPROPERTY(Replicated)
	float RemoteViewYaw;

	UPROPERTY(Replicated)
	FVector2D MovementInputVector;

	UPROPERTY(Replicated)
	ETurnDirection TurnDirection;

	UPROPERTY(Replicated)
	bool bIsToggleSlow;

	UPROPERTY(Replicated)
	bool bIsToggleSprint;

	UPROPERTY(Replicated)
	ELeanDirection LeanDirection;

	UPROPERTY(Replicated)
	float LeanTransitionDuration;

	UPROPERTY(Replicated)
	float LeaningRate;

	float DefaultCameraFOV;
	float AimCameraFOV;

	UPROPERTY(Replicated)
	ECharacterStance CurrentStance;

public:
	FORCEINLINE USkeletalMeshComponent* GetHandsMesh() const { return HandsMesh; }
	FORCEINLINE float GetDefaultAnimationTransitionDuration() const { return DefaultAnimationTransitionDuration; }
	FORCEINLINE float GetMaxLean() const { return MaxLean; }
	FORCEINLINE float GetDefaultToAimCameraFOVPercentage() const { return DefaultToAimCameraFOVPercentage; }
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE FVector2D GetMovementInputVector() const { return MovementInputVector; }
	FORCEINLINE ETurnDirection GetTurnDirection() const { return TurnDirection; }
	FORCEINLINE bool GetIsToggleSlow() const { return bIsToggleSlow; }
	FORCEINLINE bool GetIsToggleSprint() const { return bIsToggleSprint; }
	FORCEINLINE ELeanDirection GetLeanDirection() const { return LeanDirection; }
	FORCEINLINE float GetLeanTransitionDuration() const { return LeanTransitionDuration; }
	FORCEINLINE float GetLeaningRate() const { return LeaningRate; }
	FORCEINLINE ECharacterStance GetCurrentStance() const { return CurrentStance; }

};
