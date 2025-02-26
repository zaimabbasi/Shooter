// Copyright Epic Games, Inc. All Rights Reserved.


#include "Framework/ShooterGameModeBase.h"
#include "Framework/ShooterPlayerController.h"

void AShooterGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (AShooterPlayerController* ShooterPlayerController = Cast<AShooterPlayerController>(NewPlayer))
	{
		ShooterPlayerController->Init();
	}

}
