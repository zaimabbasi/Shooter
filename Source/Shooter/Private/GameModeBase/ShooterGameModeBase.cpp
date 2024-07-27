// Copyright Epic Games, Inc. All Rights Reserved.


#include "GameModeBase/ShooterGameModeBase.h"
#include "PlayerController/ShooterPlayerController.h"

void AShooterGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (AShooterPlayerController* ShooterPlayerController = Cast<AShooterPlayerController>(NewPlayer))
	{
		ShooterPlayerController->Init();
	}

}
