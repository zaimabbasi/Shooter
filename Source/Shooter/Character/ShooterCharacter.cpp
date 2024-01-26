// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Shooter/Components/CombatComponent.h"
#include "Shooter/Components/InventoryComponent.h"
#include "Shooter/Weapon/Weapon.h"

AShooterCharacter::AShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
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

}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	}

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
	
	AddMovementInput(GetActorRightVector(), CurrentValue.X);
	AddMovementInput(GetActorForwardVector(), CurrentValue.Y);

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

AWeapon* AShooterCharacter::GetEquippedWeapon()
{
	if (CombatComponent == nullptr)
	{
		return nullptr;
	}
	return CombatComponent->EquippedWeapon;
}
