// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/MagnifyingScope.h"
#include "Components/SceneCaptureComponent2D.h"

AMagnifyingScope::AMagnifyingScope()
{
	OpticCamera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("OpticCamera"));
	OpticCamera->SetupAttachment(GetMesh(), TEXT("optic_camera"));

}

FName AMagnifyingScope::GetDefaultAttachParentSocketName() const
{
	return Super::GetDefaultAttachParentSocketName();
}
