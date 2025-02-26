// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/ShooterPlayerController.h"
#include "Character/ShooterCharacter.h"

void AShooterPlayerController::Init()
{
	if (AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetCharacter()))
	{
		ShooterCharacter->Init();
	}
}
