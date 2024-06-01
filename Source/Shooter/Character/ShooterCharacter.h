// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Shooter/Types/CharacterStance.h"
#include "Shooter/Types/LeanDirection.h"
#include "Shooter/Types/TurnDirection.h"
#include "ShooterCharacter.generated.h"

class AWeapon;
class UCameraComponent;
class UCharacterDataAsset;
class UCombatComponent;
class UInputAction;
class UInputMappingContext;
class UInventoryComponent;

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

	const float DefaultAnimationTransitionDuration = 0.25f;
	const float MaxLean = 15.0f;

	const float DefaultToAimCameraFOVPercentage = 0.8f;

protected:
	virtual void BeginPlay() override;
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
	void CalculateInterpAimCameraSocketLocation(float DeltaTime);
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

	UFUNCTION()
	void OnWeaponsArrayReadyCallback();

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* LegsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* HandsMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* MovementMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* InventoryMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* CombatMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveForwardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveBackwardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipPrimaryWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipSecondaryWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleCrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleProneAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleSlowAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleSprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleLeanLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleLeanRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleAimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataAsset", meta = (AllowPrivateAccess = "true"))
	UCharacterDataAsset* CharacterDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	UCombatComponent* CombatComponent;

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

	FVector InterpAimCameraSocketLocation;

	float DefaultCameraFOV;
	float AimCameraFOV;

	UPROPERTY(Replicated)
	ECharacterStance CurrentStance;

public:
	FORCEINLINE USkeletalMeshComponent* GetHandsMesh() const { return HandsMesh; }
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE FVector2D GetMovementInputVector() const { return MovementInputVector; }
	FORCEINLINE ETurnDirection GetTurnDirection() const { return TurnDirection; }
	FORCEINLINE bool GetIsToggleSlow() const { return bIsToggleSlow; }
	FORCEINLINE bool GetIsToggleSprint() const { return bIsToggleSprint; }
	FORCEINLINE ELeanDirection GetLeanDirection() const { return LeanDirection; }
	FORCEINLINE float GetLeanTransitionDuration() const { return LeanTransitionDuration; }
	FORCEINLINE float GetLeaningRate() const { return LeaningRate; }
	FORCEINLINE FVector GetInterpAimCameraSocketLocation() const { return InterpAimCameraSocketLocation; }
	FORCEINLINE ECharacterStance GetCurrentStance() const { return CurrentStance; }
	AWeapon* GetEquippedWeapon();
	bool GetIsAiming();
	FTransform GetAimCameraSocketTransform();

};
