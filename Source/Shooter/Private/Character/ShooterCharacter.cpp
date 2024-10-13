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

FName AShooterCharacter::GetCharacterWeaponHolsterSocketName(AWeapon* Weapon) const
{
	FName WeaponHolsterSocketName = NAME_None;
	if (Weapon && InventoryComponent)
	{
		if (Weapon->IsPistol())
		{
			WeaponHolsterSocketName = PISTOL_HOLSTER_SOCKET_NAME;
		}
		else
		{
			const int8 WeaponIndex = InventoryComponent->FindWeapon(Weapon);
			if (WeaponIndex == PRIMARY_WEAPON_INDEX)
			{
				WeaponHolsterSocketName = WEAPON_HOLSTER_SOCKET_NAME;
			}
			else if (WeaponIndex != INDEX_NONE)
			{

				WeaponHolsterSocketName = WEAPON_HOLSTER_N_SOCKET_NAME(WeaponIndex);
			}
		}
	}
	return WeaponHolsterSocketName;
}

ECombatAction AShooterCharacter::GetCombatAction() const
{
	if (CombatComponent == nullptr)
	{
		return ECombatAction::CA_None;
	}
	return CombatComponent->GetCombatAction();
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

void AShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterCharacter, bIsAlterAction);
	DOREPLIFETIME(AShooterCharacter, bIsToggleSlow);
	DOREPLIFETIME(AShooterCharacter, bIsToggleSprint);
	DOREPLIFETIME(AShooterCharacter, CurrentStance);
	DOREPLIFETIME(AShooterCharacter, LeanDirection);
	DOREPLIFETIME(AShooterCharacter, LeaningRate);
	DOREPLIFETIME(AShooterCharacter, LeanTransitionDuration);
	DOREPLIFETIME(AShooterCharacter, MovementInputVector);
	DOREPLIFETIME(AShooterCharacter, RemoteViewYaw);
	DOREPLIFETIME(AShooterCharacter, TurnDirection);

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
			InventoryComponent->Server_LoadAmmoInWeaponMag(Weapon, Weapon->GetMagAmmoSpace());
		}

		for (AWeapon* Weapon : InventoryComponent->GetWeaponArray())
		{
			FName WeaponHolsterSocketName = GetCharacterWeaponHolsterSocketName(Weapon);
			Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, WeaponHolsterSocketName);
			Weapon->Server_SetIsHolster(true);
		}
	}

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
		CombatComponent->OnCombatComponentWeaponActionEnd.AddDynamic(this, &AShooterCharacter::Handle_OnCombatComponentActionEnd);
		CombatComponent->OnCombatComponentWeaponActionStart.AddDynamic(this, &AShooterCharacter::Handle_OnCombatComponentActionStart);
		CombatComponent->OnCombatComponentWeaponChamberCheck.AddDynamic(this, &AShooterCharacter::Handle_OnCombatComponentWeaponChamberCheck);
		CombatComponent->OnCombatComponentWeaponFire.AddDynamic(this, &AShooterCharacter::Handle_OnCombatComponentWeaponFire);
		CombatComponent->OnCombatComponentWeaponFireDry.AddDynamic(this, &AShooterCharacter::Handle_OnCombatComponentWeaponFireDry);
		CombatComponent->OnCombatComponentWeaponFiremode.AddDynamic(this, &AShooterCharacter::Handle_OnCombatComponentWeaponFiremode);
		CombatComponent->OnCombatComponentWeaponFiremodeCheck.AddDynamic(this, &AShooterCharacter::Handle_OnCombatComponentWeaponFiremodeCheck);
		CombatComponent->OnCombatComponentWeaponIdle.AddDynamic(this, &AShooterCharacter::Handle_OnCombatComponentIdle);
		CombatComponent->OnCombatComponentWeaponIdleToOut.AddDynamic(this, &AShooterCharacter::Handle_OnCombatComponentIdleToOut);
		CombatComponent->OnCombatComponentWeaponMagCheck.AddDynamic(this, &AShooterCharacter::Handle_OnCombatComponentWeaponMagCheck);
		CombatComponent->OnCombatComponentWeaponMagIn.AddDynamic(this, &AShooterCharacter::Handle_OnCombatComponentWeaponMagIn);
		CombatComponent->OnCombatComponentWeaponMagOut.AddDynamic(this, &AShooterCharacter::Handle_OnCombatComponentWeaponMagOut);
		CombatComponent->OnCombatComponentWeaponOut.AddDynamic(this, &AShooterCharacter::Handle_OnCombatComponentOut);
		CombatComponent->OnCombatComponentWeaponOutToIdle.AddDynamic(this, &AShooterCharacter::Handle_OnCombatComponentOutToIdle);
		CombatComponent->OnCombatComponentWeaponOutToIdleArm.AddDynamic(this, &AShooterCharacter::Handle_OnCombatComponentOutToIdleArm);
		CombatComponent->OnCombatComponentWeaponReloadCharge.AddDynamic(this, &AShooterCharacter::Handle_OnCombatComponentWeaponReloadCharge);
	}
	if (InventoryComponent)
	{
		InventoryComponent->OnInventoryComponentWeaponArrayReplicated.AddDynamic(this, &AShooterCharacter::Handle_OnInventoryComponentWeaponArrayReplicated);
	}

}

void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(CharacterLookAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnCharacterLookAction);
		EnhancedInputComponent->BindAction(CharacterMoveForwardAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnCharacterMoveForwardAction);
		EnhancedInputComponent->BindAction(CharacterMoveBackwardAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnCharacterMoveBackwardAction);
		EnhancedInputComponent->BindAction(CharacterMoveLeftAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnCharacterMoveLeftAction);
		EnhancedInputComponent->BindAction(CharacterMoveRightAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnCharacterMoveRightAction);
		EnhancedInputComponent->BindAction(CharacterEquipPrimaryWeaponAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnCharacterEquipPrimaryWeaponAction);
		EnhancedInputComponent->BindAction(CharacterEquipSecondaryWeaponAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnCharacterEquipSecondaryWeaponAction);
		EnhancedInputComponent->BindAction(CharacterHolsterWeaponAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnCharacterHolsterWeaponAction);
		EnhancedInputComponent->BindAction(CharacterCrouchAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnCharacterCrouchAction);
		EnhancedInputComponent->BindAction(CharacterProneAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnCharacterProneAction);
		EnhancedInputComponent->BindAction(CharacterSlowAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnCharacterSlowAction);
		EnhancedInputComponent->BindAction(CharacterSprintAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnCharacterSprintAction);
		EnhancedInputComponent->BindAction(CharacterLeanLeftAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnCharacterLeanLeftAction);
		EnhancedInputComponent->BindAction(CharacterLeanRightAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnCharacterLeanRightAction);
		EnhancedInputComponent->BindAction(CharacterAimAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnCharacterAimAction);
		EnhancedInputComponent->BindAction(CharacterAlterAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnCharacterAlterAction);
		EnhancedInputComponent->BindAction(WeaponChamberCheckAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnWeaponChamberCheckAction);
		EnhancedInputComponent->BindAction(WeaponReloadAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnWeaponReloadAction);
		EnhancedInputComponent->BindAction(WeaponFiremodeAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnWeaponFiremodeAction);
		EnhancedInputComponent->BindAction(WeaponFireAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &AShooterCharacter::OnWeaponFireAction);
	}

}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAO_Pitch(DeltaTime);

	UpdateMovement(DeltaTime);

	UpdateCameraFOV(DeltaTime);

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
			EnhancedInputLocalPlayerSubsystem->AddMappingContext(OtherMappingContext.LoadSynchronous(), 0);
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
			//Server_EquipWeaponProgressive(PrimaryWeapon);
		}
	}

}

void AShooterCharacter::Handle_OnCombatComponentActionEnd(AWeapon* Weapon)
{

}

void AShooterCharacter::Handle_OnCombatComponentActionStart(AWeapon* Weapon)
{
}

void AShooterCharacter::Handle_OnCombatComponentIdle(AWeapon* Weapon)
{
}

void AShooterCharacter::Handle_OnCombatComponentIdleToOut(AWeapon* Weapon)
{
	if (HasAuthority() && CombatComponent)
	{
		CombatComponent->Server_Out();
	}
}

void AShooterCharacter::Handle_OnCombatComponentOut(AWeapon* Weapon)
{
	if (HasAuthority() && CombatComponent)
	{
		if (CombatComponent->GetEquippedWeapon())
		{
			CombatComponent->Server_UnequipWeapon(GetMesh(), GetCharacterWeaponHolsterSocketName(CombatComponent->GetEquippedWeapon()));
		}
		if (NextWeaponToEquip)
		{
			Server_EquipWeaponProgressive(NextWeaponToEquip);
		}
		else
		{
			Server_HolsterWeaponProgressive();
		}
	}
}

void AShooterCharacter::Handle_OnCombatComponentOutToIdle(AWeapon* Weapon)
{
	if (HasAuthority() && CombatComponent)
	{
		CombatComponent->Server_Idle();
	}
}

void AShooterCharacter::Handle_OnCombatComponentOutToIdleArm(AWeapon* Weapon)
{
	if (HasAuthority() && CombatComponent)
	{
		CombatComponent->Server_Idle();
	}
}

void AShooterCharacter::Handle_OnCombatComponentWeaponChamberCheck(AWeapon* Weapon)
{
}

void AShooterCharacter::Handle_OnCombatComponentWeaponFire(AWeapon* Weapon)
{
	
}

void AShooterCharacter::Handle_OnCombatComponentWeaponFireDry(AWeapon* Weapon)
{
}

void AShooterCharacter::Handle_OnCombatComponentWeaponFiremode(AWeapon* Weapon)
{
	if (HasAuthority() && CombatComponent)
	{
		CombatComponent->Server_Idle();
	}
}

void AShooterCharacter::Handle_OnCombatComponentWeaponFiremodeCheck(AWeapon* Weapon)
{
	if (HasAuthority() && CombatComponent)
	{
		CombatComponent->Server_Idle();
	}
}

void AShooterCharacter::Handle_OnCombatComponentWeaponMagCheck(AWeapon* Weapon)
{
}

void AShooterCharacter::Handle_OnCombatComponentWeaponMagIn(AWeapon* Weapon)
{
	if (HasAuthority() && Weapon && CombatComponent)
	{
		if (Weapon->DoesNeedCharge())
		{
			CombatComponent->Server_WeaponReloadCharge();
		}
		else
		{
			CombatComponent->Server_ActionEnd();
		}
	}

}

void AShooterCharacter::Handle_OnCombatComponentWeaponMagOut(AWeapon* Weapon)
{
	if (HasAuthority() && Weapon && CombatComponent && InventoryComponent)
	{
		const uint8 WeaponMagSpace = Weapon->GetMagAmmoSpace();
		const uint8 WeaponAmmoInInventory = InventoryComponent->GetAmmoAtIndex(InventoryComponent->FindWeapon(Weapon));
		const uint8 WeaponMagAmmoCountToLoad = FMath::Min(WeaponMagSpace, WeaponAmmoInInventory);
		InventoryComponent->Server_LoadAmmoInWeaponMag(Weapon, WeaponMagAmmoCountToLoad);
		CombatComponent->Server_WeaponMagIn();
	}
}

void AShooterCharacter::Handle_OnCombatComponentWeaponReloadCharge(AWeapon* Weapon)
{
}

void AShooterCharacter::Handle_OnHandsAnimInstanceIdle()
{
}

void AShooterCharacter::Handle_OnHandsAnimInstanceIdleToOut()
{
	if (HasAuthority() && CombatComponent)
	{
		CombatComponent->Server_Out();
	}
}

void AShooterCharacter::Handle_OnHandsAnimInstanceOut()
{
	if (HasAuthority() && NextWeaponToEquip)
	{
		Server_EquipWeaponProgressive(NextWeaponToEquip);
	}
}

void AShooterCharacter::Handle_OnHandsAnimInstanceOutToIdle()
{
	if (HasAuthority() && CombatComponent)
	{
		CombatComponent->Server_Idle();
	}
}

void AShooterCharacter::Handle_OnInventoryComponentWeaponArrayReplicated()
{
	if (InventoryComponent && CombatComponent)
	{
		AWeapon* PrimaryWeapon = InventoryComponent->GetWeaponAtIndex(PRIMARY_WEAPON_INDEX);
		if (PrimaryWeapon && PrimaryWeapon != CombatComponent->GetEquippedWeapon())
		{
			//Server_EquipWeaponProgressive(PrimaryWeapon);
		}
	}
}

void AShooterCharacter::OnCharacterAimAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CombatComponent && !(bIsToggleSprint && MovementInputVector.Y == 1.0))
	{
		CombatComponent->Server_SetIsAiming(CurrentValue);
	}
}

void AShooterCharacter::OnCharacterAlterAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	Server_SetIsAlterAction(CurrentValue);
}

void AShooterCharacter::OnCharacterCrouchAction(const FInputActionValue& Value)
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

void AShooterCharacter::OnCharacterEquipPrimaryWeaponAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue && InventoryComponent && CombatComponent)
	{
		AWeapon* PrimaryWeapon = InventoryComponent->GetWeaponAtIndex(PRIMARY_WEAPON_INDEX);
		if (PrimaryWeapon && PrimaryWeapon != CombatComponent->GetEquippedWeapon())
		{
			Server_EquipWeaponProgressive(PrimaryWeapon);
		}
	}
}

void AShooterCharacter::OnCharacterEquipSecondaryWeaponAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue && InventoryComponent && CombatComponent)
	{
		AWeapon* SecondaryWeapon = InventoryComponent->GetWeaponAtIndex(SECONDARY_WEAPON_INDEX);
		if (SecondaryWeapon && SecondaryWeapon != CombatComponent->GetEquippedWeapon())
		{
			Server_EquipWeaponProgressive(SecondaryWeapon);
		}
	}
}

void AShooterCharacter::OnCharacterHolsterWeaponAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue && CombatComponent && CombatComponent->GetEquippedWeapon())
	{
		Server_HolsterWeaponProgressive();
	}
}

void AShooterCharacter::OnCharacterLeanLeftAction(const FInputActionValue& Value)
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

void AShooterCharacter::OnCharacterLeanRightAction(const FInputActionValue& Value)
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

void AShooterCharacter::OnCharacterLookAction(const FInputActionValue& Value)
{
	const FVector2D CurrentValue = Value.Get<FVector2D>();

	AddControllerYawInput(CurrentValue.X);
	AddControllerPitchInput(CurrentValue.Y);

}

void AShooterCharacter::OnCharacterMoveBackwardAction(const FInputActionValue& Value)
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

void AShooterCharacter::OnCharacterMoveForwardAction(const FInputActionValue& Value)
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

void AShooterCharacter::OnCharacterMoveLeftAction(const FInputActionValue& Value)
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

void AShooterCharacter::OnCharacterMoveRightAction(const FInputActionValue& Value)
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

void AShooterCharacter::OnCharacterProneAction(const FInputActionValue& Value)
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

void AShooterCharacter::OnCharacterSlowAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	SetIsToggleSlow(CurrentValue);

	if (CurrentValue && bIsToggleSprint)
	{
		SetIsToggleSprint(false);
	}
}

void AShooterCharacter::OnCharacterSprintAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	SetIsToggleSprint(CurrentValue);

	if (CurrentValue && MovementInputVector.Y == 1.0)
	{
		TransitionToSprint();
	}
}

void AShooterCharacter::OnWeaponChamberCheckAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue && CombatComponent && bIsAlterAction)
	{
		CombatComponent->Server_WeaponChamberCheck();
	}
}

void AShooterCharacter::OnWeaponFireAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CombatComponent)
	{
		CombatComponent->Server_WeaponFire(CurrentValue);
	}
}

void AShooterCharacter::OnWeaponFiremodeAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue && CombatComponent)
	{
		if (!bIsAlterAction)
		{
			CombatComponent->Server_WeaponFiremode();
		}
		else
		{
			CombatComponent->Server_WeaponFiremodeCheck();
		}
	}
}

void AShooterCharacter::OnWeaponReloadAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue && CombatComponent)
	{
		if (!bIsAlterAction)
		{
			AWeapon* EquippedWeapon = CombatComponent->GetEquippedWeapon();
			if (EquippedWeapon && InventoryComponent)
			{
				const uint8 WeaponMagSpace = EquippedWeapon->GetMagAmmoSpace();
				const uint8 WeaponAmmoInInventory = InventoryComponent->GetAmmoAtIndex(InventoryComponent->FindWeapon(EquippedWeapon));
				if (WeaponMagSpace > 0 && WeaponAmmoInInventory > 0)
				{
					CombatComponent->Server_WeaponMagOut();
				}
			}
		}
		else
		{
			CombatComponent->Server_WeaponMagCheck();
		}
	}
}

void AShooterCharacter::Server_EquipWeaponProgressive_Implementation(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr || CombatComponent == nullptr)
	{
		return;
	}
	const ECombatAction CombatAction = GetCombatAction();
	if (NextWeaponToEquip == nullptr && CombatAction == ECombatAction::CA_Idle)
	{
		NextWeaponToEquip = WeaponToEquip;
		CombatComponent->Server_IdleToOut();
	}
	else if (NextWeaponToEquip && CombatAction == ECombatAction::CA_Out)
	{
		CombatComponent->Server_EquipWeapon(NextWeaponToEquip, HandsMesh, BASE_HUMAN_RIBCAGE_SOCKET_NAME);
		CombatComponent->Server_OutToIdle();
		NextWeaponToEquip = nullptr;
	}
}

void AShooterCharacter::Server_HolsterWeaponProgressive_Implementation()
{
	if (CombatComponent == nullptr)
	{
		return;
	}
	const ECombatAction CombatAction = GetCombatAction();
	if (CombatAction == ECombatAction::CA_Idle)
	{
		NextWeaponToEquip = nullptr;
		CombatComponent->Server_IdleToOut();
	}
	else if (NextWeaponToEquip == nullptr && CombatAction == ECombatAction::CA_Out)
	{
		CombatComponent->Server_OutToIdle();
	}
}

void AShooterCharacter::Server_SetCurrentStance_Implementation(ECharacterStance NewStance)
{
	CurrentStance = NewStance;
}

void AShooterCharacter::Server_SetIsAlterAction_Implementation(bool bAlterAction)
{
	bIsAlterAction = bAlterAction;
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

void AShooterCharacter::Server_SetMovementInputVector_Implementation(float MovementInputX, float MovementInputY)
{
	MovementInputVector.Set(MovementInputX, MovementInputY);
}

void AShooterCharacter::Server_SetRemoteViewYaw_Implementation(float RemoteYaw)
{
	RemoteViewYaw = RemoteYaw;
}

void AShooterCharacter::Server_SetTurnDirection_Implementation(ETurnDirection NewTurnDirection)
{
	TurnDirection = NewTurnDirection;
}

void AShooterCharacter::SetCurrentStance(ECharacterStance NewStance)
{
	CurrentStance = NewStance;
	Server_SetCurrentStance(NewStance);
}

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

void AShooterCharacter::TransitionToSprint()
{
	if (CurrentStance == ECharacterStance::CS_Crouch)
	{
		UnCrouch();
	}
	SetCurrentStance(ECharacterStance::CS_Stand);

	if (GetIsAiming())
	{
		CombatComponent->Server_SetIsAiming(false);
	}

	if (LeanDirection != ELeanDirection::LD_None)
	{
		SetLeanDirection(ELeanDirection::LD_None);
		SetLeanTransitionDuration(DefaultAnimationTransitionDuration);
		SetLeaningRate(MaxLean / DefaultAnimationTransitionDuration);
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
