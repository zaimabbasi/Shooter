// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Actor/Weapon.h"
#include "ActorComponent/CombatComponent.h"
#include "ActorComponent/InventoryComponent.h"
#include "AnimInstance/HandsAnimInstance.h"
#include "DataAsset/CharacterDataAsset.h"
#include "Enum/CharacterStance.h"
#include "Enum/LeanDirection.h"
#include "Enum/TurnDirection.h"
#include "Type/ShooterNameType.h"

AShooterCharacter::AShooterCharacter()
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
	GetCharacterMovement()->RotationRate.Yaw = 180.0f;

}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAO_Pitch(DeltaTime);

	UpdateMovement(DeltaTime);

	UpdateCameraFOV(DeltaTime);

}

void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(LookAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnLookAction);
		EnhancedInputComponent->BindAction(MoveForwardAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnMoveForwardAction);
		EnhancedInputComponent->BindAction(MoveBackwardAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnMoveBackwardAction);
		EnhancedInputComponent->BindAction(MoveLeftAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnMoveLeftAction);
		EnhancedInputComponent->BindAction(MoveRightAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnMoveRightAction);
		EnhancedInputComponent->BindAction(EquipPrimaryWeaponAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnEquipPrimaryWeaponAction);
		EnhancedInputComponent->BindAction(EquipSecondaryWeaponAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnEquipSecondaryWeaponAction);
		EnhancedInputComponent->BindAction(HolsterEquippedWeaponAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnHolsterEquippedWeaponAction);
		EnhancedInputComponent->BindAction(ToggleCrouchAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnToggleCrouchAction);
		EnhancedInputComponent->BindAction(ToggleProneAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnToggleProneAction);
		EnhancedInputComponent->BindAction(ToggleSlowAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnToggleSlowAction);
		EnhancedInputComponent->BindAction(ToggleSprintAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnToggleSprintAction);
		EnhancedInputComponent->BindAction(ToggleLeanLeftAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnToggleLeanLeftAction);
		EnhancedInputComponent->BindAction(ToggleLeanRightAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnToggleLeanRightAction);
		EnhancedInputComponent->BindAction(ToggleAimAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnToggleAimAction);
		EnhancedInputComponent->BindAction(ReloadWeaponAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnReloadWeaponAction);
	}

}

void AShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterCharacter, RemoteViewYaw);
	DOREPLIFETIME(AShooterCharacter, MovementInputVector);
	DOREPLIFETIME(AShooterCharacter, TurnDirection);
	DOREPLIFETIME(AShooterCharacter, bIsToggleSlow);
	DOREPLIFETIME(AShooterCharacter, bIsToggleSprint);
	DOREPLIFETIME(AShooterCharacter, LeanDirection);
	DOREPLIFETIME(AShooterCharacter, LeanTransitionDuration);
	DOREPLIFETIME(AShooterCharacter, LeaningRate);
	DOREPLIFETIME(AShooterCharacter, CurrentStance);

}

void AShooterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (USkeletalMeshComponent* CharacterMesh = GetMesh())
	{
		CharacterMesh->SetSkeletalMeshAsset(USkeletalMergingLibrary::MergeMeshes(CharacterMeshMergeParams));
	}
	if (HandsMesh)
	{
		if (UHandsAnimInstance* HandsAnimInstance = Cast<UHandsAnimInstance>(HandsMesh->GetAnimInstance()))
		{
			HandsAnimInstance->OnHandsAnimInstanceIdle.AddDynamic(this, &AShooterCharacter::Handle_OnHandsAnimInstanceIdle);
			HandsAnimInstance->OnHandsAnimInstanceIdleToOut.AddDynamic(this, &AShooterCharacter::Handle_OnHandsAnimInstanceIdleToOut);
			HandsAnimInstance->OnHandsAnimInstanceOut.AddDynamic(this, &AShooterCharacter::Handle_OnHandsAnimInstanceOut);
			HandsAnimInstance->OnHandsAnimInstanceOutToIdle.AddDynamic(this, &AShooterCharacter::Handle_OnHandsAnimInstanceOutToIdle);
		}
	}
	if (FirstPersonCamera)
	{
		DefaultCameraFOV = FirstPersonCamera->FieldOfView;
		AimCameraFOV = DefaultToAimCameraFOVPercentage * DefaultCameraFOV;
	}

	if (CombatComponent)
	{
		CombatComponent->OnCombatComponentWeaponIdleToOut.AddDynamic(this, &AShooterCharacter::Handle_OnCombatComponentWeaponIdleToOut);
		CombatComponent->OnCombatComponentWeaponOut.AddDynamic(this, &AShooterCharacter::Handle_OnCombatComponentWeaponOut);
	}
	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryComponentWeaponArrayReplicated.AddDynamic(this, &AShooterCharacter::Handle_OnInventoryComponentWeaponArrayReplicated);
	}

}

void AShooterCharacter::Init()
{
	if (InventoryComponent)
	{
		if (const UCharacterDataAsset* LoadedCharacterDataAsset = CharacterDataAsset.LoadSynchronous())
		{
			InventoryComponent->Init(LoadedCharacterDataAsset->InventoryParams);
		}

		for (AWeapon* Weapon : InventoryComponent->GetWeaponArray())
		{
			InventoryComponent->LoadAmmoInWeaponMag(Weapon, Weapon->GetMagAmmoSpace());
			Weapon->LoadAmmoInChamber();
		}

		for (AWeapon* Weapon : InventoryComponent->GetWeaponArray())
		{
			FName WeaponHolsterSocketName = GetCharacterWeaponHolsterSocketName(Weapon);
			Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, WeaponHolsterSocketName);
			Weapon->SetIsHolster(true);
		}
	}

}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			EnhancedInputLocalPlayerSubsystem->AddMappingContext(MovementMappingContext.LoadSynchronous(), 0);
			EnhancedInputLocalPlayerSubsystem->AddMappingContext(InventoryMappingContext.LoadSynchronous(), 0);
			EnhancedInputLocalPlayerSubsystem->AddMappingContext(CombatMappingContext.LoadSynchronous(), 0);
		}
	}

	const bool bIsLocallyControlled = IsLocallyControlled();
	if (bIsLocallyControlled)
	{
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
			//HandsMesh->SetVisibility(false);
		}
	}

	if (bIsLocallyControlled && InventoryComponent && CombatComponent)
	{
		AWeapon* PrimaryWeapon = InventoryComponent->GetWeaponAtIndex(PRIMARY_WEAPON_INDEX);
		if (PrimaryWeapon && PrimaryWeapon != CombatComponent->GetEquippedWeapon())
		{
			/*NextWeaponToEquip = PrimaryWeapon;
			CombatComponent->SetCombatAction(ECombatAction::CA_IdleToOut);*/
		}
	}

}

void AShooterCharacter::Handle_OnInventoryComponentWeaponArrayReplicated()
{
	if (InventoryComponent && CombatComponent)
	{
		AWeapon* PrimaryWeapon = InventoryComponent->GetWeaponAtIndex(PRIMARY_WEAPON_INDEX);
		if (PrimaryWeapon && PrimaryWeapon != CombatComponent->GetEquippedWeapon())
		{
			/*NextWeaponToEquip = PrimaryWeapon;
			CombatComponent->SetCombatAction(ECombatAction::CA_IdleToOut);*/
		}
	}
}

void AShooterCharacter::Handle_OnCombatComponentWeaponIdleToOut(AWeapon* Weapon)
{
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));
	if (IsLocallyControlled() && CombatComponent)
	{
		CombatComponent->WeaponAttach(GetMesh(), GetCharacterWeaponHolsterSocketName(Weapon));
		if (Weapon)
		{
			Weapon->SetIsHolster(true);
			Weapon->SetCombatAction(ECombatAction::CA_Out);
		}

		CombatComponent->SetEquippedWeapon(NextWeaponToEquip);
		CombatComponent->WeaponAttach(HandsMesh, CHARACTER_BASE_HUMAN_RIBCAGE_SOCKET_NAME);
		if (NextWeaponToEquip)
		{
			NextWeaponToEquip->SetIsHolster(false);
		}
		CombatComponent->SetCombatAction(ECombatAction::CA_OutToIdle);
		NextWeaponToEquip = nullptr;
	}
}

void AShooterCharacter::Handle_OnCombatComponentWeaponOut(AWeapon* Weapon)
{
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));
	/*if (IsLocallyControlled() && CombatComponent)
	{
		CombatComponent->WeaponAttach(GetMesh(), GetCharacterWeaponHolsterSocketName(Weapon));

		CombatComponent->SetEquippedWeapon(NextWeaponToEquip);
		CombatComponent->WeaponAttach(HandsMesh, CHARACTER_BASE_HUMAN_RIBCAGE_SOCKET_NAME);
		CombatComponent->SetCombatAction(ECombatAction::CA_OutToIdle);
		NextWeaponToEquip = nullptr;
	}*/
}

void AShooterCharacter::Handle_OnHandsAnimInstanceIdle()
{
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));
}

void AShooterCharacter::Handle_OnHandsAnimInstanceIdleToOut()
{
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));
	if (IsLocallyControlled() && CombatComponent)
	{
		CombatComponent->SetCombatAction(ECombatAction::CA_Out);
	}
}

void AShooterCharacter::Handle_OnHandsAnimInstanceOut()
{
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));
	if (IsLocallyControlled() && CombatComponent && NextWeaponToEquip)
	{
		CombatComponent->SetEquippedWeapon(NextWeaponToEquip);
		CombatComponent->WeaponAttach(HandsMesh, CHARACTER_BASE_HUMAN_RIBCAGE_SOCKET_NAME);
		NextWeaponToEquip->SetIsHolster(false);
		CombatComponent->SetCombatAction(ECombatAction::CA_OutToIdle);
		NextWeaponToEquip = nullptr;
	}
}

void AShooterCharacter::Handle_OnHandsAnimInstanceOutToIdle()
{
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));
	if (IsLocallyControlled() && CombatComponent)
	{
		CombatComponent->SetCombatAction(ECombatAction::CA_Idle);
	}
}

FName AShooterCharacter::GetCharacterWeaponHolsterSocketName(AWeapon* Weapon) const
{
	FName WeaponHolsterSocketName = NAME_None;
	if (Weapon && InventoryComponent)
	{
		if (Weapon->IsPistol())
		{
			WeaponHolsterSocketName = CHARACTER_PISTOL_HOLSTER_SOCKET_NAME;
		}
		else
		{
			const int8 WeaponIndex = InventoryComponent->FindWeapon(Weapon);
			if (WeaponIndex == PRIMARY_WEAPON_INDEX)
			{
				WeaponHolsterSocketName = CHARACTER_PRIMARY_WEAPON_HOLSTER_SOCKET_NAME;
			}
			else if (WeaponIndex != INDEX_NONE)
			{
				
				WeaponHolsterSocketName = CHARACTER_SECONDARY_WEAPON_HOLSTER_SOCKET_NAME(WeaponIndex);
			}
		}
	}
	return WeaponHolsterSocketName;
}

void AShooterCharacter::OnLookAction(const FInputActionValue& Value)
{
	const FVector2D CurrentValue = Value.Get<FVector2D>();
	
	AddControllerYawInput(CurrentValue.X);
	AddControllerPitchInput(CurrentValue.Y);

}

void AShooterCharacter::OnMoveForwardAction(const FInputActionValue& Value)
{
	const float CurrentValue = Value.Get<float>();
	float MovementInputY = CurrentValue;
	if (CurrentValue)
	{
		if (MovementInputVector.Y == -1.0)
		{
			MovementInputY = 0.0f;
		}
	}
	else
	{
		if (MovementInputVector.Y == 0.0)
		{
			MovementInputY = -1.0f;
		}
	}
	SetMovementInputVector(MovementInputVector.X, MovementInputY);

	if (MovementInputY == 1.0 && bIsToggleSprint)
	{
		TransitionToSprint();
	}
}

void AShooterCharacter::OnMoveBackwardAction(const FInputActionValue& Value)
{
	const float CurrentValue = Value.Get<float>();
	float MovementInputY = CurrentValue;
	if (CurrentValue)
	{
		if (MovementInputVector.Y == 1.0)
		{
			MovementInputY = 0.0f;
		}
	}
	else
	{
		if (MovementInputVector.Y == 0.0)
		{
			MovementInputY = 1.0f;
		}
	}
	SetMovementInputVector(MovementInputVector.X, MovementInputY);

	if (MovementInputY == 1.0 && bIsToggleSprint)
	{
		TransitionToSprint();
	}
}

void AShooterCharacter::OnMoveLeftAction(const FInputActionValue& Value)
{
	const float CurrentValue = Value.Get<float>();
	float MovementInputX = CurrentValue;
	if (CurrentValue)
	{
		if (MovementInputVector.X == 1.0)
		{
			MovementInputX = 0.0f;
		}
	}
	else
	{
		if (MovementInputVector.X == 0.0)
		{
			MovementInputX = 1.0f;
		}
	}
	SetMovementInputVector(MovementInputX, MovementInputVector.Y);
}

void AShooterCharacter::OnMoveRightAction(const FInputActionValue& Value)
{
	const float CurrentValue = Value.Get<float>();
	float MovementInputX = CurrentValue;
	if (CurrentValue)
	{
		if (MovementInputVector.X == -1.0)
		{
			MovementInputX = 0.0f;
		}
	}
	else
	{
		if (MovementInputVector.X == 0.0)
		{
			MovementInputX = -1.0f;
		}
	}
	SetMovementInputVector(MovementInputX, MovementInputVector.Y);
}

void AShooterCharacter::OnEquipPrimaryWeaponAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue && InventoryComponent && CombatComponent)
	{
		AWeapon* PrimaryWeapon = InventoryComponent->GetWeaponAtIndex(PRIMARY_WEAPON_INDEX);
		if (PrimaryWeapon && PrimaryWeapon != CombatComponent->GetEquippedWeapon())
		{
			NextWeaponToEquip = PrimaryWeapon;
			CombatComponent->SetCombatAction(ECombatAction::CA_IdleToOut);
		}
	}
}

void AShooterCharacter::OnEquipSecondaryWeaponAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue && InventoryComponent && CombatComponent)
	{
		AWeapon* SecondaryWeapon = InventoryComponent->GetWeaponAtIndex(SECONDARY_WEAPON_INDEX);
		if (SecondaryWeapon && SecondaryWeapon != CombatComponent->GetEquippedWeapon())
		{
			NextWeaponToEquip = SecondaryWeapon;
			CombatComponent->SetCombatAction(ECombatAction::CA_IdleToOut);
		}
	}
}

void AShooterCharacter::OnHolsterEquippedWeaponAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue && CombatComponent && CombatComponent->GetEquippedWeapon())
	{
		NextWeaponToEquip = nullptr;
		CombatComponent->SetCombatAction(ECombatAction::CA_IdleToOut);
	}
}

void AShooterCharacter::OnToggleCrouchAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		ECharacterStance NewStance;
		if (CurrentStance != ECharacterStance::CS_Crouch)
		{
			NewStance = ECharacterStance::CS_Crouch;
			Crouch();
		}
		else
		{
			NewStance = ECharacterStance::CS_Stand;
			UnCrouch();
		}
		SetCurrentStance(NewStance);

		if (bIsToggleSprint)
		{
			SetIsToggleSprint(false);
		}
	}
}

void AShooterCharacter::OnToggleProneAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		ECharacterStance NewStance;
		if (CurrentStance != ECharacterStance::CS_Prone)
		{
			NewStance = ECharacterStance::CS_Prone;
			if (CurrentStance == ECharacterStance::CS_Crouch)
			{
				UnCrouch();
			}
		}
		else
		{
			NewStance = ECharacterStance::CS_Stand;
		}
		SetCurrentStance(NewStance);

		if (bIsToggleSprint)
		{
			SetIsToggleSprint(false);
		}
	}
}

void AShooterCharacter::OnToggleSlowAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	SetIsToggleSlow(CurrentValue);
	
	if (CurrentValue && bIsToggleSprint)
	{
		SetIsToggleSprint(false);
	}
}

void AShooterCharacter::OnToggleSprintAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	SetIsToggleSprint(CurrentValue);
	
	if (CurrentValue && MovementInputVector.Y == 1.0)
	{
		TransitionToSprint();
	}
}

void AShooterCharacter::OnToggleLeanLeftAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue && !(bIsToggleSprint && MovementInputVector.Y == 1.0))
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
		SetLeanDirection(NewLeanDirection);
		SetLeanTransitionDuration(TransitionDuration);
		SetLeaningRate(MaxLeanRotation / TransitionDuration);
	}
}

void AShooterCharacter::OnToggleLeanRightAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue && !(bIsToggleSprint && MovementInputVector.Y == 1.0))
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
		SetLeanDirection(NewLeanDirection);
		SetLeanTransitionDuration(TransitionDuration);
		SetLeaningRate(MaxLeanRotation / TransitionDuration);
	}
}

void AShooterCharacter::OnToggleAimAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CombatComponent && !(bIsToggleSprint && MovementInputVector.Y == 1.0))
	{
		CombatComponent->SetIsAiming(CurrentValue);
	}
}

void AShooterCharacter::OnReloadWeaponAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue && CombatComponent)
	{
		CombatComponent->ReloadWeapon();
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

void AShooterCharacter::TransitionToSprint()
{
	if (CurrentStance == ECharacterStance::CS_Crouch)
	{
		UnCrouch();
	}
	SetCurrentStance(ECharacterStance::CS_Stand);

	if (GetIsAiming())
	{
		CombatComponent->SetIsAiming(false);
	}

	if (LeanDirection != ELeanDirection::LD_None)
	{
		SetLeanDirection(ELeanDirection::LD_None);
		SetLeanTransitionDuration(DefaultAnimationTransitionDuration);
		SetLeaningRate(MaxLean / DefaultAnimationTransitionDuration);
	}
}

void AShooterCharacter::SetMovementInputVector(float MovementInputX, float MovementInputY)
{
	MovementInputVector.Set(MovementInputX, MovementInputY);
	Server_SetMovementInputVector(MovementInputX, MovementInputY);
}

//void AShooterCharacter::SetTurnDirection(ETurnDirection NewTurnDirection)
//{
//	TurnDirection = NewTurnDirection;
//	Server_SetTurnDirection(NewTurnDirection);
//}

void AShooterCharacter::SetIsToggleSlow(bool bToggleSlow)
{
	bIsToggleSlow = bToggleSlow;
	Server_SetIsToggleSlow(bToggleSlow);
}

void AShooterCharacter::SetIsToggleSprint(bool bToggleSprint)
{
	bIsToggleSprint = bToggleSprint;
	Server_SetIsToggleSprint(bToggleSprint);
}

void AShooterCharacter::SetLeanDirection(ELeanDirection NewLeanDirection)
{
	LeanDirection = NewLeanDirection;
	Server_SetLeanDirection(NewLeanDirection);
}

void AShooterCharacter::SetLeanTransitionDuration(float NewLeanTransitionDuration)
{
	LeanTransitionDuration = NewLeanTransitionDuration;
	Server_SetLeanTransitionDuration(NewLeanTransitionDuration);
}

void AShooterCharacter::SetLeaningRate(float NewLeaningRate)
{
	LeaningRate = NewLeaningRate;
	Server_SetLeaningRate(NewLeaningRate);
}

void AShooterCharacter::SetCurrentStance(ECharacterStance NewStance)
{
	CurrentStance = NewStance;
	Server_SetCurrentStance(NewStance);
}

void AShooterCharacter::Server_SetRemoteViewYaw_Implementation(float RemoteYaw)
{
	RemoteViewYaw = RemoteYaw;
}

void AShooterCharacter::Server_SetMovementInputVector_Implementation(float MovementInputX, float MovementInputY)
{
	MovementInputVector.Set(MovementInputX, MovementInputY);
}

void AShooterCharacter::Server_SetTurnDirection_Implementation(ETurnDirection NewTurnDirection)
{
	TurnDirection = NewTurnDirection;
}

void AShooterCharacter::Server_SetIsToggleSlow_Implementation(bool bToggleSlow)
{
	bIsToggleSlow = bToggleSlow;
}

void AShooterCharacter::Server_SetIsToggleSprint_Implementation(bool bToggleSprint)
{
	bIsToggleSprint = bToggleSprint;
}

void AShooterCharacter::Server_SetLeanDirection_Implementation(ELeanDirection NewLeanDirection)
{
	LeanDirection = NewLeanDirection;
}

void AShooterCharacter::Server_SetLeanTransitionDuration_Implementation(float NewTransitionDuration)
{
	LeanTransitionDuration = NewTransitionDuration;
}

void AShooterCharacter::Server_SetLeaningRate_Implementation(float NewLeaningRate)
{
	LeaningRate = NewLeaningRate;
}

void AShooterCharacter::Server_SetCurrentStance_Implementation(ECharacterStance NewStance)
{
	CurrentStance = NewStance;
}

AWeapon* AShooterCharacter::GetEquippedWeapon() const
{
	if (CombatComponent == nullptr)
	{
		return nullptr;
	}
	return CombatComponent->GetEquippedWeapon();
}

bool AShooterCharacter::GetIsAiming() const
{
	if (CombatComponent == nullptr)
	{
		return false;
	}
	return CombatComponent->GetIsAiming();
}

ECombatAction AShooterCharacter::GetCombatAction() const
{
	if (CombatComponent == nullptr)
	{
		return ECombatAction::CA_None;
	}
	return CombatComponent->GetCombatAction();
}
