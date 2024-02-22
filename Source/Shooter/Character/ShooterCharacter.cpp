// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Shooter/Components/CombatComponent.h"
#include "Shooter/Components/InventoryComponent.h"
#include "Shooter/Weapon/Weapon.h"

AShooterCharacter::AShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	LegsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegsMesh"));
	LegsMesh->SetupAttachment(GetRootComponent());
	LegsMesh->SetCastShadow(false);

	HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandsMesh"));
	HandsMesh->SetupAttachment(GetMesh());
	HandsMesh->SetCastShadow(false);

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(HandsMesh, TEXT("Camera_animated"));
	FirstPersonCamera->bConstrainAspectRatio = true;

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetIsReplicated(true);

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComponent->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ControlMovement(DeltaTime);

}

void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Look);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Move);
		EnhancedInputComponent->BindAction(EquipPrimaryWeaponAction, ETriggerEvent::Triggered, this, &AShooterCharacter::EquipPrimaryWeapon);
		EnhancedInputComponent->BindAction(EquipSecondaryWeaponAction, ETriggerEvent::Triggered, this, &AShooterCharacter::EquipSecondaryWeapon);
		EnhancedInputComponent->BindAction(ToggleCrouchUnCrouchAction, ETriggerEvent::Triggered, this, &AShooterCharacter::ToggleCrouchUncrouch);
	}

}

void AShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterCharacter, RemoteViewYaw);
	DOREPLIFETIME(AShooterCharacter, MovementInputVector);

}

void AShooterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (InventoryComponent)
	{
		InventoryComponent->OwningCharacter = this;
		if (CharacterDataAsset)
		{
			InventoryComponent->InventoryDataArray = CharacterDataAsset->InventoryDataArray;
		}
	}
	if (CombatComponent)
	{
		CombatComponent->OwningCharacter = this;
	}
}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			EnhancedInputLocalPlayerSubsystem->AddMappingContext(MovementMappingContext, 0);
			EnhancedInputLocalPlayerSubsystem->AddMappingContext(InventoryMappingContext, 0);
		}
	}
	
	if (IsLocallyControlled())
	{
		if (LegsMesh)
		{
			LegsMesh->HideBoneByName(TEXT("Base-HumanSpine3"), EPhysBodyOp::PBO_None);
		}
		if (USkeletalMeshComponent* CharacterMesh = GetMesh())
		{
			CharacterMesh->SetVisibility(false);
			CharacterMesh->SetCastHiddenShadow(true);
		}
	}
	else
	{
		if (LegsMesh)
		{
			LegsMesh->DestroyComponent();
		}
		if (HandsMesh)
		{
			HandsMesh->SetVisibility(false);
		}
	}

	if (HasAuthority())
	{
		if (InventoryComponent && CombatComponent)
		{
			CombatComponent->Server_SetEquippedWeapon(InventoryComponent->GetWeaponAtIndex(InventoryComponent->PRIMARY_WEAPON_INDEX));
			InventoryComponent->Server_SetCurrentIndex(InventoryComponent->PRIMARY_WEAPON_INDEX);
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

	Server_SetMovementInputVector(CurrentValue);

}

void AShooterCharacter::EquipPrimaryWeapon(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		if (InventoryComponent && CombatComponent)
		{
			AWeapon* PrimaryWeapon = InventoryComponent->GetWeaponAtIndex(InventoryComponent->PRIMARY_WEAPON_INDEX);
			if (CombatComponent->EquippedWeapon != PrimaryWeapon)
			{
				CombatComponent->Server_SetEquippedWeapon(PrimaryWeapon);
				InventoryComponent->Server_SetCurrentIndex(InventoryComponent->PRIMARY_WEAPON_INDEX);
			}
		}
	}
}

void AShooterCharacter::EquipSecondaryWeapon(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		if (InventoryComponent && CombatComponent)
		{
			AWeapon* SecondaryWeapon = InventoryComponent->GetWeaponAtIndex(InventoryComponent->SECONDARY_WEAPON_INDEX);
			if (CombatComponent->EquippedWeapon != SecondaryWeapon)
			{
				CombatComponent->Server_SetEquippedWeapon(SecondaryWeapon);
				InventoryComponent->Server_SetCurrentIndex(InventoryComponent->SECONDARY_WEAPON_INDEX);
			}
		}
	}
}

void AShooterCharacter::ToggleCrouchUncrouch(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		if (bIsCrouched)
		{
			UnCrouch();
		}
		else
		{
			Crouch();
		}
	}
}

void AShooterCharacter::ControlMovement(float DeltaTime)
{
	FRotator CurrentAimRotation = IsLocallyControlled() ? GetControlRotation() : GetBaseAimRotation();
	
	CalculateAO_Yaw(CurrentAimRotation, DeltaTime);
	CalculateAO_Pitch(CurrentAimRotation, DeltaTime);

}

void AShooterCharacter::CalculateAO_Yaw(FRotator CurrentAimRotation, float DeltaTime)
{

}

void AShooterCharacter::CalculateAO_Pitch(FRotator CurrentAimRotation, float DeltaTime)
{
	if (IsLocallyControlled())
	{
		AO_Pitch = CurrentAimRotation.Pitch;
	}
	else
	{
		AO_Pitch = FMath::RInterpTo(FRotator(AO_Pitch, 0.0, 0.0), FRotator(CurrentAimRotation.Pitch, 0.0, 0.0), DeltaTime, 15.0f).Pitch;
	}

	if (AO_Pitch > 90.0f)
	{
		FVector2D InRange = FVector2D(270.0, 360.0);
		FVector2D OutRange = FVector2D(-90.0, 0.0);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void AShooterCharacter::Server_SetRemoteViewYaw_Implementation(float RemoteYaw)
{
	RemoteViewYaw = RemoteYaw;
}

void AShooterCharacter::Server_SetMovementInputVector_Implementation(FVector2D MovementInput)
{
	MovementInputVector = MovementInput;
}

AWeapon* AShooterCharacter::GetEquippedWeapon()
{
	if (CombatComponent == nullptr)
	{
		return nullptr;
	}
	return CombatComponent->EquippedWeapon;
}
