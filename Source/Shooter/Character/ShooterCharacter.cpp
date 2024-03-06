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

AShooterCharacter::AShooterCharacter() : 
	TurnDirection(ETurnDirection::TD_None),
	LeanDirection(ELeanDirection::LD_None)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	bUseControllerRotationYaw = false;

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
		EnhancedInputComponent->BindAction(ToggleCrouchAction, ETriggerEvent::Triggered, this, &AShooterCharacter::ToggleCrouch);
		EnhancedInputComponent->BindAction(ToggleSlowAction, ETriggerEvent::Triggered, this, &AShooterCharacter::ToggleSlow);
		EnhancedInputComponent->BindAction(ToggleSprintAction, ETriggerEvent::Triggered, this, &AShooterCharacter::ToggleSprint);
		EnhancedInputComponent->BindAction(ToggleLeanLeftAction, ETriggerEvent::Triggered, this, &AShooterCharacter::ToggleLeanLeft);
		EnhancedInputComponent->BindAction(ToggleLeanRightAction, ETriggerEvent::Triggered, this, &AShooterCharacter::ToggleLeanRight);
	}

}

void AShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterCharacter, RemoteViewYaw);
	DOREPLIFETIME(AShooterCharacter, MovementInputVector);
	DOREPLIFETIME(AShooterCharacter, bIsSlow);
	DOREPLIFETIME(AShooterCharacter, bIsSprinting);
	DOREPLIFETIME(AShooterCharacter, LeanDirection);

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

		LastAimRotation = FRotator(0.0, GetControlRotation().Yaw, 0.0);
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

		LastAimRotation = FRotator(0.0, GetBaseAimRotation().Yaw, 0.0);
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

	MovementInputVector.Set(CurrentValue.X, CurrentValue.Y);
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

void AShooterCharacter::ToggleCrouch(const FInputActionValue& Value)
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

void AShooterCharacter::ToggleSlow(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		const bool bSlow = !bIsSlow;
		bIsSlow = bSlow;
		Server_SetIsSlow(bSlow);
	}
	
}

void AShooterCharacter::ToggleSprint(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	
	bIsSprinting = CurrentValue;
	Server_SetIsSprinting(CurrentValue);
}

void AShooterCharacter::ToggleLeanLeft(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		ELeanDirection Direction = ELeanDirection::LD_Left;
		if (LeanDirection == ELeanDirection::LD_Left)
		{
			Direction = ELeanDirection::LD_None;
		}
		LeanDirection = Direction;
		Server_SetLeanDirection(Direction);
	}
}

void AShooterCharacter::ToggleLeanRight(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		ELeanDirection Direction = ELeanDirection::LD_Right;
		if (LeanDirection == ELeanDirection::LD_Right)
		{
			Direction = ELeanDirection::LD_None;
		}
		LeanDirection = Direction;
		Server_SetLeanDirection(Direction);
	}
}

void AShooterCharacter::ControlMovement(float DeltaTime)
{
	FRotator CurrentAimRotation = IsLocallyControlled() ? GetControlRotation() : GetBaseAimRotation();
	bool bInputMove = MovementInputVector.Size() > 0.0 ? true : false;
	if (bInputMove)
	{
		TurnDirection = ETurnDirection::TD_None;
	}

	AO_Yaw = UKismetMathLibrary::NormalizedDeltaRotator(FRotator(0.0, CurrentAimRotation.Yaw, 0.0), FRotator(0.0, LastAimRotation.Yaw, 0.0)).Yaw;
	if (HasAuthority() || IsLocallyControlled())
	{
		Server_SetRemoteViewYaw(AO_Yaw);
	}
	else
	{
		AO_Yaw = RemoteViewYaw;
	}

	CalculateAO_Pitch(DeltaTime);

	if (AO_Yaw < -90.0f)
	{
		TurnDirection = ETurnDirection::TD_Left;
	}
	else if (AO_Yaw > 90.0f)
	{
		TurnDirection = ETurnDirection::TD_Right;
	}

	if (TurnDirection != ETurnDirection::TD_None)
	{
		float DeltaActorRotationYaw = UKismetMathLibrary::NormalizedDeltaRotator(GetActorRotation(), LastActorRotation).Yaw;
		LastAimRotation.Add(0.0, DeltaActorRotationYaw, 0.0);
		AO_Yaw -= DeltaActorRotationYaw;
		if (FMath::IsNearlyZero(DeltaActorRotationYaw))
		{
			if (!bIsTurning)
			{
				bIsTurning = true;
			}
			else
			{
				TurnDirection = ETurnDirection::TD_None;
			}
		}
	}
	else
	{
		bIsTurning = false;
	}

	if (bInputMove)
	{
		float YawRotationRate = GetCharacterMovement()->RotationRate.Yaw * DeltaTime;
		if (FMath::Abs(AO_Yaw) < YawRotationRate)
		{
			YawRotationRate = AO_Yaw;
		}
		else if (AO_Yaw < 0.0)
		{
			YawRotationRate *= -1.0f;
		}
		LastAimRotation.Add(0.0, YawRotationRate, 0.0);
		AddActorLocalRotation(FRotator(0.0, YawRotationRate, 0.0));
		AO_Yaw -= YawRotationRate;
	}

	LastActorRotation = GetActorRotation();
}

void AShooterCharacter::CalculateAO_Pitch(float DeltaTime)
{
	if (IsLocallyControlled())
	{
		AO_Pitch = GetControlRotation().Pitch;
	}
	else
	{
		AO_Pitch = FMath::RInterpTo(FRotator(AO_Pitch, 0.0, 0.0), FRotator(GetBaseAimRotation().Pitch, 0.0, 0.0), DeltaTime, 15.0f).Pitch;
	}

	if (AO_Pitch > 90.0f)
	{
		FVector2D InRange = FVector2D(270.0, 360.0);
		FVector2D OutRange = FVector2D(-90.0, 0.0);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void AShooterCharacter::Server_SetLeanDirection_Implementation(ELeanDirection Direction)
{
	LeanDirection = Direction;
}

void AShooterCharacter::Server_SetIsSprinting_Implementation(bool bSprinting)
{
	bIsSprinting = bSprinting;
}

void AShooterCharacter::Server_SetIsSlow_Implementation(bool bSlow)
{
	bIsSlow = bSlow;
}

void AShooterCharacter::Server_SetRemoteViewYaw_Implementation(float RemoteYaw)
{
	RemoteViewYaw = RemoteYaw;
}

void AShooterCharacter::Server_SetMovementInputVector_Implementation(FVector2D MovementInput)
{
	MovementInputVector.Set(MovementInput.X, MovementInput.Y);
}

AWeapon* AShooterCharacter::GetEquippedWeapon()
{
	if (CombatComponent == nullptr)
	{
		return nullptr;
	}
	return CombatComponent->EquippedWeapon;
}
