// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

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
	FirstPersonCamera->bUsePawnControlRotation = true;

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

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Look);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Move);
	}

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			EnhancedInputLocalPlayerSubsystem->AddMappingContext(MovementMappingContext, 0);
		}
	}

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

void AShooterCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D CurrentValue = Value.Get<FVector2D>();
	
	AddControllerYawInput(CurrentValue.X);
	AddControllerPitchInput(CurrentValue.Y);

}

void AShooterCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D CurrentValue = Value.Get<FVector2D>();
	
	AddMovementInput(GetActorRightVector(), CurrentValue.X);
	AddMovementInput(GetActorForwardVector(), CurrentValue.Y);

}

