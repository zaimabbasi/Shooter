// Fill out your copyright notice in the Description page of Project Settings.


#include "Mod/MagnifyingScope.h"
#include "Components/SceneCaptureComponent2D.h"

AMagnifyingScope::AMagnifyingScope()
{
	OpticCamera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("OpticCamera"));
	OpticCamera->SetupAttachment(GetMesh(), TEXT("optic_camera"));
	OpticCamera->FOVAngle = 60.0f;
	OpticCamera->bMainViewResolution = true;
	OpticCamera->bIgnoreScreenPercentage = true;

	Magnification = 1.0f;
}

void AMagnifyingScope::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (OpticCamera)
	{
		float Y = FMath::Tan(FMath::DegreesToRadians(OpticCamera->FOVAngle) / 2.0f);
		OpticCamera->FOVAngle = FMath::RadiansToDegrees(2.0f * FMath::Atan(Y / Magnification));
	}
}

FName AMagnifyingScope::GetDefaultAttachParentSocketName() const
{
	return Super::GetDefaultAttachParentSocketName();
}
