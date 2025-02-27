// Copyright Epic Games, Inc. All Rights Reserved.


#include "GameFramework/ShooterGameModeBase.h"
#include "GameFramework/ShooterPlayerController.h"

void AShooterGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (AShooterPlayerController* ShooterPlayerController = Cast<AShooterPlayerController>(NewPlayer))
	{
		ShooterPlayerController->Init();
	}

}
