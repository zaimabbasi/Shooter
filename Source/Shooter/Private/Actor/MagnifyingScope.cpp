// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/MagnifyingScope.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Actor/Weapon.h"

AMagnifyingScope::AMagnifyingScope()
{
	OpticCamera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("OpticCamera"));
	OpticCamera->SetupAttachment(GetMesh(), TEXT("optic_camera"));

	/*LensMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LensMesh"));
	LensMesh->SetupAttachment(GetMesh());
	LensMesh->SetEnableGravity(false);
	LensMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LensMesh->SetOnlyOwnerSee(true);*/

}

FName AMagnifyingScope::GetDefaultAttachParentSocketName() const
{
	return Super::GetDefaultAttachParentSocketName();
}

void AMagnifyingScope::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (OpticCamera)
	{	
		OpticCamera->HiddenComponents.Add(GetMesh());
	}
}
