// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFramework/ShooterPlayerController.h"
#include "Character/ShooterCharacter.h"

void AShooterPlayerController::Init()
{
	if (ShooterCharacter)
	{
		ShooterCharacter->Init();
	}
}

void AShooterPlayerController::UpdateRotation(float DeltaTime)
{
	Super::UpdateRotation(DeltaTime);

	if (ShooterCharacter)
	{
		ShooterCharacter->OnControllerUpdated(DeltaTime);
	}
}

void AShooterPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	ShooterCharacter = Cast<AShooterCharacter>(GetCharacter());
}

void AShooterPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	ShooterCharacter = Cast<AShooterCharacter>(GetCharacter());
}
