// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ShooterCharacterMovementComponent.generated.h"

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

	uint8 bWantsToSprint : 1;

};

UCLASS()
class SHOOTER_API UShooterCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UShooterCharacterMovementComponent();
	bool CanSprintInCurrentState() const;
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	uint8 bWantsToSprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter Character Movement", meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float MaxWalkSpeedWalk;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooter Character Movement", meta = (ClampMin = "0", UIMin = "0", ForceUnits = "cm/s"))
	float MaxWalkSpeedSprint;

};
