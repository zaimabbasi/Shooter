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

	bUseControllerRotationYaw = false;

	LegsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegsMesh"));
	LegsMesh->SetupAttachment(GetMesh());
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
	GetCharacterMovement()->RotationRate.Yaw = 180.0f;

}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAO_Pitch(DeltaTime);

	UpdateMovement(DeltaTime);
	
	CalculateInterpAimCameraSocketLocation(DeltaTime);

	UpdateCameraFOV(DeltaTime);

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
		EnhancedInputComponent->BindAction(ToggleAimAction, ETriggerEvent::Triggered, this, &AShooterCharacter::ToggleAim);
	}

}

void AShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterCharacter, RemoteViewYaw);
	DOREPLIFETIME(AShooterCharacter, MovementInputVector);
	DOREPLIFETIME(AShooterCharacter, TurnDirection);
	DOREPLIFETIME(AShooterCharacter, bIsSlow);
	DOREPLIFETIME(AShooterCharacter, bIsSprinting);
	DOREPLIFETIME(AShooterCharacter, LeanDirection);
	DOREPLIFETIME(AShooterCharacter, LeanTransitionDuration);
	DOREPLIFETIME(AShooterCharacter, LeaningRate);

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

	if (FirstPersonCamera)
	{
		DefaultCameraFOV = FirstPersonCamera->FieldOfView;
		AimCameraFOV = DefaultToAimCameraFOVPercentage * DefaultCameraFOV;
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
			EnhancedInputLocalPlayerSubsystem->AddMappingContext(CombatMappingContext, 0);
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
		ELeanDirection NewLeanDirection = ELeanDirection::LD_Left;
		float TransitionDuration = DefaultAnimationTransitionDuration;
		float MaxLeanRotation = MaxLean;
		if (LeanDirection == ELeanDirection::LD_Left)
		{
			NewLeanDirection = ELeanDirection::LD_None;
		}
		else if (LeanDirection == ELeanDirection::LD_Right)
		{
			TransitionDuration = DefaultAnimationTransitionDuration * 2.0f;
			MaxLeanRotation = MaxLean * 2.0f;
		}
		LeanDirection = NewLeanDirection;
		Server_SetLeanDirection(NewLeanDirection);

		LeanTransitionDuration = TransitionDuration;
		Server_SetLeanTransitionDuration(TransitionDuration);

		const float Rate = MaxLeanRotation / LeanTransitionDuration;
		LeaningRate = Rate;
		Server_SetLeaningRate(Rate);
	}
}

void AShooterCharacter::ToggleLeanRight(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		ELeanDirection NewLeanDirection = ELeanDirection::LD_Right;
		float TransitionDuration = DefaultAnimationTransitionDuration;
		float MaxLeanRotation = MaxLean;
		if (LeanDirection == ELeanDirection::LD_Right)
		{
			NewLeanDirection = ELeanDirection::LD_None;
		}
		else if (LeanDirection == ELeanDirection::LD_Left)
		{
			TransitionDuration = DefaultAnimationTransitionDuration * 2.0f;
			MaxLeanRotation = MaxLean * 2.0f;
		}
		LeanDirection = NewLeanDirection;
		Server_SetLeanDirection(NewLeanDirection);

		LeanTransitionDuration = TransitionDuration;
		Server_SetLeanTransitionDuration(TransitionDuration);

		const float Rate = MaxLeanRotation / LeanTransitionDuration;
		LeaningRate = Rate;
		Server_SetLeaningRate(Rate);
	}
}

void AShooterCharacter::ToggleAim(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();

	if (CombatComponent)
	{
		CombatComponent->SetIsAiming(CurrentValue);
	}
}

void AShooterCharacter::UpdateMovement(float DeltaTime)
{
	float bMovementInput = MovementInputVector.Size() > 0.0 ? true : false;
	float CurrentYaw = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation()).Yaw;
	if (Controller == nullptr)
	{
		CurrentYaw = RemoteViewYaw;
	}

	if (FMath::IsNearlyZero(CurrentYaw, UE_KINDA_SMALL_NUMBER) || bMovementInput)
	{
		TurnDirection = ETurnDirection::TD_None;
	}
	else if (CurrentYaw < -90.0f)
	{
		TurnDirection = ETurnDirection::TD_Left;
	}
	else if (CurrentYaw > 90.0f)
	{
		TurnDirection = ETurnDirection::TD_Right;
	}

	if (TurnDirection != ETurnDirection::TD_None || bMovementInput)
	{	
		GetCharacterMovement()->bUseControllerDesiredRotation = true;

		float ExceedingYaw = 0.0f;
		if (CurrentYaw < -90.0f)
		{
			ExceedingYaw = CurrentYaw + 90.0f;
		}
		else if (CurrentYaw > 90.0f)
		{
			ExceedingYaw = CurrentYaw - 90.0f;
		}

		if (bMovementInput)
		{
			float YawRotationStep = GetCharacterMovement()->RotationRate.Yaw * DeltaTime;
			if (FMath::Abs(CurrentYaw) < YawRotationStep)
			{
				YawRotationStep = CurrentYaw;
			}
			else if (CurrentYaw < 0.0)
			{
				YawRotationStep *= -1.0f;
			}
			ExceedingYaw += YawRotationStep;
		}

		AddActorLocalRotation(FRotator(0.0, ExceedingYaw, 0.0));
		CurrentYaw -= ExceedingYaw;
	}
	else
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
	}

	if (IsLocallyControlled())
	{
		AO_Yaw = CurrentYaw;
	}
	else
	{
		AO_Yaw = FMath::FInterpTo(AO_Yaw, CurrentYaw, DeltaTime, 15.0f);
	}
	
	if (IsLocallyControlled())
	{
		Server_SetRemoteViewYaw(AO_Yaw);
		Server_SetTurnDirection(TurnDirection);
	}
	
}

void AShooterCharacter::UpdateAO_Pitch(float DeltaTime)
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

void AShooterCharacter::CalculateInterpAimCameraSocketLocation(float DeltaTime)
{
	FVector AimCameraSocketLocation = GetAimCameraSocketTransform().GetLocation();
	bool bIsAiming = GetIsAiming();
	float AimSpeed = 10.0f;
	FVector AimStep = FVector::ZeroVector;
	if (bIsAiming)
	{
		AimStep = AimCameraSocketLocation * AimSpeed * DeltaTime;
		if (AimCameraSocketLocation.Size() < AimStep.Size())
		{
			AimStep = AimCameraSocketLocation;
		}
		InterpAimCameraSocketLocation += AimStep;
	}
	else
	{
		AimStep = InterpAimCameraSocketLocation * AimSpeed * DeltaTime;
		if (InterpAimCameraSocketLocation.Size() < AimStep.Size())
		{
			AimStep = InterpAimCameraSocketLocation;
		}
		InterpAimCameraSocketLocation -= AimStep;
	}
}

void AShooterCharacter::UpdateCameraFOV(float DeltaTime)
{
	if (FirstPersonCamera == nullptr)
	{
		return;
	}
	bool bIsAiming = GetIsAiming();
	if (!bIsAiming && FirstPersonCamera->FieldOfView == DefaultCameraFOV)
	{
		return;
	}
	if (bIsAiming && FirstPersonCamera->FieldOfView == AimCameraFOV)
	{
		return;
	}
	float AimSpeed = 50.0f;
	float FOVStep = AimSpeed * DeltaTime;
	if (bIsAiming)
	{
		FirstPersonCamera->FieldOfView -= FOVStep;
		if (FirstPersonCamera->FieldOfView < AimCameraFOV)
		{
			FirstPersonCamera->FieldOfView = AimCameraFOV;
		}
	}
	else
	{
		FirstPersonCamera->FieldOfView += FOVStep;
		if (FirstPersonCamera->FieldOfView > DefaultCameraFOV)
		{
			FirstPersonCamera->FieldOfView = DefaultCameraFOV;
		}
	}
}

void AShooterCharacter::Server_SetRemoteViewYaw_Implementation(float RemoteYaw)
{
	RemoteViewYaw = RemoteYaw;
}

void AShooterCharacter::Server_SetMovementInputVector_Implementation(FVector2D MovementInput)
{
	MovementInputVector.Set(MovementInput.X, MovementInput.Y);
}

void AShooterCharacter::Server_SetTurnDirection_Implementation(ETurnDirection NewTurnDirection)
{
	TurnDirection = NewTurnDirection;
}

void AShooterCharacter::Server_SetIsSlow_Implementation(bool bSlow)
{
	bIsSlow = bSlow;
}

void AShooterCharacter::Server_SetIsSprinting_Implementation(bool bSprinting)
{
	bIsSprinting = bSprinting;
}

void AShooterCharacter::Server_SetLeanDirection_Implementation(ELeanDirection NewLeanDirection)
{
	LeanDirection = NewLeanDirection;
}

void AShooterCharacter::Server_SetLeanTransitionDuration_Implementation(float TransitionDuration)
{
	LeanTransitionDuration = TransitionDuration;
}

void AShooterCharacter::Server_SetLeaningRate_Implementation(float Rate)
{
	LeaningRate = Rate;
}

AWeapon* AShooterCharacter::GetEquippedWeapon()
{
	if (CombatComponent == nullptr)
	{
		return nullptr;
	}
	return CombatComponent->EquippedWeapon;
}

bool AShooterCharacter::GetIsAiming()
{
	if (CombatComponent == nullptr)
	{
		return false;
	}
	return CombatComponent->bIsAiming;
}

FTransform AShooterCharacter::GetAimCameraSocketTransform()
{
	if (CombatComponent == nullptr)
	{
		return FTransform();
	}
	return CombatComponent->AimCameraSocketTransform;
}
