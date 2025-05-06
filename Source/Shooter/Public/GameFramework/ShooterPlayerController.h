// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

class AShooterCharacter;

UCLASS()
class SHOOTER_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void UpdateRotation(float DeltaTime);
	virtual void OnRep_Pawn();
	virtual void Init();

protected:
	virtual void OnPossess(APawn* aPawn) override;

private:
	TObjectPtr<AShooterCharacter> ShooterCharacter;

};
