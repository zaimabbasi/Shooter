// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
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

	const float DefaultAnimationTransitionDuration = 0.2f;

protected:
	virtual void BeginPlay() override;
	void Look(const FInputActionValue& Value);
	void Move(const FInputActionValue& Value);
	void EquipPrimaryWeapon(const FInputActionValue& Value);
	void EquipSecondaryWeapon(const FInputActionValue& Value);
	void ToggleCrouch(const FInputActionValue& Value);
	void ToggleSlow(const FInputActionValue& Value);
	void ToggleSprint(const FInputActionValue& Value);
	void ToggleLeanLeft(const FInputActionValue& Value);
	void ToggleLeanRight(const FInputActionValue& Value);
	void ControlMovement(float DeltaTime);
	void CalculateAO_Pitch(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void Server_SetRemoteViewYaw(float RemoteYaw);

	UFUNCTION(Server, Reliable)
	void Server_SetMovementInputVector(FVector2D MovementInput);

	UFUNCTION(Server, Reliable)
	void Server_SetIsSlow(bool bSlow);

	UFUNCTION(Server, Reliable)
	void Server_SetIsSprinting(bool bSprinting);

	UFUNCTION(Server, Reliable)
	void Server_SetLeanDirection(ELeanDirection NewLeanDirection);

	UFUNCTION(Server, Reliable)
	void Server_SetLeanTransitionDuration(float TransitionDuration);

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
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipPrimaryWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipSecondaryWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleCrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleSlowAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleSprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleLeanLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleLeanRightAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataAsset", meta = (AllowPrivateAccess = "true"))
	UCharacterDataAsset* CharacterDataAsset;

	UInventoryComponent* InventoryComponent;

	UCombatComponent* CombatComponent;

	float AO_Yaw;
	float AO_Pitch;

	UPROPERTY(Replicated)
	float RemoteViewYaw;

	UPROPERTY(Replicated)
	FVector2D MovementInputVector;

	FRotator LastAimRotation;
	FRotator LastActorRotation;

	ETurnDirection TurnDirection;

	bool bIsTurning;

	UPROPERTY(Replicated)
	bool bIsSlow;

	UPROPERTY(Replicated)
	bool bIsSprinting;

	UPROPERTY(Replicated)
	ELeanDirection LeanDirection;

	UPROPERTY(Replicated)
	float LeanTransitionDuration;

public:
	FORCEINLINE USkeletalMeshComponent* GetHandsMesh() const { return HandsMesh; }
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE FVector2D GetMovementInputVector() const { return MovementInputVector; }
	FORCEINLINE ETurnDirection GetTurnDirection() const { return TurnDirection; }
	FORCEINLINE bool GetIsSlow() const { return bIsSlow; }
	FORCEINLINE bool GetIsSprinting() const { return bIsSprinting; }
	FORCEINLINE ELeanDirection GetLeanDirection() const { return LeanDirection; }
	FORCEINLINE float GetLeanTransitionDuration() const { return LeanTransitionDuration; }
	AWeapon* GetEquippedWeapon();

};
