// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ShooterCharacterMovementComponent.generated.h"

class AShooterCharacter;
class FNetworkPredictionData_Client;

class FNetworkPredictionData_Client_ShooterCharacter : public FNetworkPredictionData_Client_Character
{
	using Super = FNetworkPredictionData_Client_Character;

public:
	FNetworkPredictionData_Client_ShooterCharacter(const UCharacterMovementComponent& ClientMovement);
	virtual FSavedMovePtr AllocateNewMove() override;
};

class FSavedMove_ShooterCharacter : public FSavedMove_Character
{
	using Super = FSavedMove_Character;

public:
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
	virtual void Clear() override;
	virtual uint8 GetCompressedFlags() const override;
	virtual void PrepMoveFor(ACharacter* C) override;
	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;

public:
	uint8 bWantsToProne : 1;
	uint8 bWantsToSlow : 1;
	uint8 bWantsToSprint : 1;

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMovementComponentSprintSignature);

UCLASS()
class SHOOTER_API UShooterCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UShooterCharacterMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	//virtual void ApplyVelocityBraking(float DeltaTime, float Friction, float BrakingDeceleration) override;
	//virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;
	virtual bool CanAttemptJump() const override;
	virtual bool CanProneInCurrentState() const;
	virtual bool CanSlowInCurrentState() const;
	virtual bool CanSprintInCurrentState() const;
	virtual bool CanWalkOffLedges() const override;
	//virtual float GetMaxAcceleration() const override;
	//virtual float GetMaxBrakingDeceleration() const override;
	virtual float GetMaxSpeed() const override;
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	UFUNCTION(BlueprintGetter)
	float GetPronedHalfHeight() const;

	//float GetRemainingProneLockCooldown() const;
	//float GetTimestamp() const;
	virtual bool HasValidData() const override;

	/*UFUNCTION(BlueprintPure, Category = "Character Movement")
	bool IsProneLocked() const;*/

	virtual bool IsProning() const;
	//bool IsProneLockOnTimer() const;
	virtual bool IsSlowing() const;
	virtual bool IsSprinting() const;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void PostLoad() override;
	virtual void Prone(bool bClientSimulation = false);

	UFUNCTION(BlueprintSetter)
	void SetPronedHalfHeight(const float NewValue);

	//void SetProneLock(bool bLock);
	virtual void SetUpdatedComponent(USceneComponent* NewUpdatedComponent) override;
	virtual void Slow();
	virtual void Sprint();
	virtual void UnProne(bool bClientSimulation = false);
	virtual void UnSlow();
	virtual void UnSprint();
	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

protected:
	virtual bool ClientUpdatePositionAfterServerUpdate() override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

public:
	FOnMovementComponentSprintSignature OnMovementComponentSprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter Character Movement: Walking")
	uint8 bCanWalkOffLedgesWhenProning : 1;

	/*UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	uint8 bProneLocked : 1;*/

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Shooter Character Movement (General Settings)")
	uint8 bWantsToProne : 1;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Shooter Character Movement (General Settings)")
	uint8 bWantsToSlow : 1;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Shooter Character Movement (General Settings)")
	uint8 bWantsToSprint : 1;

	/*UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float BrakingDecelerationProned;*/

	/*UPROPERTY(Category = "Character Movement (General Settings)", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0", EditCondition = "bUseSeparateBrakingFriction"))
	float BrakingFrictionProned;*/

	/*UPROPERTY(Category = "Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float GroundFrictionProned;*/

	/*UPROPERTY(Category = "Character Movement (General Settings)", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MaxAccelerationProned;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter Character Movement: Walking", meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float MaxWalkSpeedProned;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter Character Movement: Walking", meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float MaxWalkSpeedCrouchedSlow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter Character Movement: Walking", meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float MaxWalkSpeedSlow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter Character Movement: Walking", meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float MaxWalkSpeedSprint;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter Character Movement: Walking", meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float MaxWalkSpeedWalk;*/

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter Character Movement: Walking", meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float MaxWalkSpeedWalkCrouched;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter Character Movement (General Settings)", BlueprintSetter = SetPronedHalfHeight, BlueprintGetter = GetPronedHalfHeight, meta = (ClampMin = "0", UIMin = "0", ForceUnits = cm))
	float PronedHalfHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter Character Movement (General Settings)", meta = (ClampMin = "0", UIMin = "0", ForceUnits = cm))
	float PronedRadius;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter Character Movement (General Settings)", meta = (ClampMin = "0", UIMin = "0", ForceUnits = cm))
	float ProneLockDuration;*/

protected:
	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<AShooterCharacter> ShooterCharacterOwner;

	//float ProneLockTimestamp = -1.f;

};
