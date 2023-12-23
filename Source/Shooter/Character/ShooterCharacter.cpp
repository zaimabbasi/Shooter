// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Camera/CameraComponent.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	CharacterMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	CharacterMesh1P->SetupAttachment(GetRootComponent());
	CharacterMesh1P->SetCastShadow(false);

	HandsMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandsMesh1P"));
	HandsMesh1P->SetupAttachment(GetRootComponent());
	HandsMesh1P->SetCastShadow(false);

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(HandsMesh1P, TEXT("Camera_animatedSocket"));
	FirstPersonCamera->bConstrainAspectRatio = true;

}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled())
	{
		if (USkeletalMeshComponent* CharacterMesh3P = GetMesh())
		{
			CharacterMesh3P->SetVisibility(false);
			CharacterMesh3P->SetCastHiddenShadow(true);
		}
		if (CharacterMesh1P)
		{
			CharacterMesh1P->HideBoneByName(TEXT("Base-HumanSpine3"), EPhysBodyOp::PBO_None);
		}
	}
	else
	{
		if (CharacterMesh1P)
		{
			CharacterMesh1P->DestroyComponent();
		}
		if (HandsMesh1P)
		{
			HandsMesh1P->DestroyComponent();
		}
	}
	
}

