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
#include "AnimInstance/CharacterAnimInstance.h"
#include "AnimInstance/HandsAnimInstance.h"
#include "DataAsset/CharacterDataAsset.h"
#include "Enum/CharacterStance.h"
#include "Enum/LeanDirection.h"
#include "Enum/TurnDirection.h"
#include "Type/ShooterNameType.h"

AShooterCharacter::AShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
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

float AShooterCharacter::GetAO_Pitch(float CurrentPitch, float DeltaTime) const
{
	float Pitch = IsLocallyControlled() ? GetControlRotation().Pitch : FMath::RInterpTo(FRotator(CurrentPitch, 0.0, 0.0), FRotator(GetBaseAimRotation().Pitch, 0.0, 0.0), DeltaTime, 10.0f).Pitch;
	if (Pitch > 90.0f)
	{
		FVector2D InRange = FVector2D(270.0, 360.0);
		FVector2D OutRange = FVector2D(-90.0, 0.0);
		Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, Pitch);
	}
	return Pitch;
}

float AShooterCharacter::GetAO_Yaw(float CurrentYaw, float DeltaTime)
{
	float ControlRotationYaw = GetController() ? GetControlRotation().Yaw : FRotator::DecompressAxisFromByte(RemoteViewYaw);
	float Yaw = UKismetMathLibrary::NormalizedDeltaRotator(FRotator(0.0, ControlRotationYaw, 0.0), FRotator(0.0, GetActorRotation().Yaw, 0.0)).Yaw;
	if (!IsLocallyControlled())
	{
		Yaw = FMath::RInterpTo(FRotator(0.0, CurrentYaw, 0.0), FRotator(0.0, Yaw, 0.0), DeltaTime, 10.0f).Yaw;
	}
	return Yaw;
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

FVector AShooterCharacter::GetCurrentAcceleration() const
{
	return GetCharacterMovement() ? GetCharacterMovement()->GetCurrentAcceleration() : FVector::ZeroVector;
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
	//DOREPLIFETIME_CONDITION(AShooterCharacter, bIsRemoteAccelerating, COND_SkipOwner);
	//DOREPLIFETIME_CONDITION(AShooterCharacter, RemoteAcceleration, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AShooterCharacter, RemoteViewYaw, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AShooterCharacter, TurnDirection, COND_SkipOwner);

}

ETurnDirection AShooterCharacter::GetTurnDirection(float CurrentYaw)
{
	if (GetVelocity().SizeSquared2D() > 0.0f)
	{
		if (TurnDirection != ETurnDirection::TD_None)
		{
			TurnDirection = ETurnDirection::TD_None;
		}
	}
	else
	{
		if (CurrentYaw < -90.0f && TurnDirection != ETurnDirection::TD_Left)
		{
			TurnDirection = ETurnDirection::TD_Left;
		}
		else if (CurrentYaw > 90.0f && TurnDirection != ETurnDirection::TD_Right)
		{
			TurnDirection = ETurnDirection::TD_Right;
		}
	}
	return TurnDirection;
}

bool AShooterCharacter::GetUseControllerDesiredRotation() const
{
	return GetCharacterMovement() && GetCharacterMovement()->bUseControllerDesiredRotation;
}

float AShooterCharacter::GetYawExceedingMaxLimit(float CurrentYaw) const
{
	if (CurrentYaw < -90.0f)
	{
		return CurrentYaw + 90.0f;
	}
	else if (CurrentYaw > 90.0f)
	{
		return CurrentYaw - 90.0f;
	}
	return 0.0f;
}

//bool AShooterCharacter::HasVelocity() const
//{
//	return GetVelocity().SizeSquared2D() > 0.0f;
//}

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

//bool AShooterCharacter::IsAccelerating() const
//{
//	return GetController() ? GetCharacterMovement() && GetCharacterMovement()->GetCurrentAcceleration().SizeSquared2D() > 0.0f : RemoteAcceleration.SizeSquared2D() > 0.0f;
//}

void AShooterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (USkeletalMeshComponent* CharacterMesh = GetMesh())
	{
		CharacterMesh->SetSkeletalMeshAsset(USkeletalMergingLibrary::MergeMeshes(CharacterMeshMergeParams));
		if (UCharacterAnimInstance* CharacterAnimInstance = Cast<UCharacterAnimInstance>(CharacterMesh->GetAnimInstance()))
		{
			CharacterAnimInstance->OnCharacterAnimInstanceTurnInPlace.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceTurnInPlace);
			CharacterAnimInstance->OnCharacterAnimInstanceControllerDesiredRotationNeeded.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceControllerDesiredRotationNeeded);
		}
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

void AShooterCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	if (HasAuthority())
	{
		/*if (GetCharacterMovement())
		{
			SetIsRemoteAccelerating(GetCharacterMovement()->GetCurrentAcceleration().SizeSquared2D() > 0.0f);
			SetRemoteAcceleration(GetCharacterMovement()->GetCurrentAcceleration());
		}*/
		if (GetController())
		{
			SetRemoteViewYaw(GetController()->GetControlRotation().Yaw);
		}
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

bool AShooterCharacter::CanPerformCrouchAction() const
{
	return TurnDirection == ETurnDirection::TD_None;
}

bool AShooterCharacter::CanPerformMoveAction() const
{
	return true;
}

bool AShooterCharacter::CanPerformProneAction() const
{
	return TurnDirection == ETurnDirection::TD_None;
}

float AShooterCharacter::GetMaxWalkSpeed() const
{
	if (const UCharacterDataAsset* LoadedCharacterDataAsset = CharacterDataAsset.LoadSynchronous())
	{
		return LoadedCharacterDataAsset->MovementParams.MaxWalkSpeed;
	}
	return 0.0f;
}

float AShooterCharacter::GetMaxWalkSpeedCrouched() const
{
	if (const UCharacterDataAsset* LoadedCharacterDataAsset = CharacterDataAsset.LoadSynchronous())
	{
		return LoadedCharacterDataAsset->MovementParams.MaxWalkSpeedCrouched;
	}
	return 0.0f;
}

float AShooterCharacter::GetMaxWalkSpeedCrouchedSlow() const
{
	if (const UCharacterDataAsset* LoadedCharacterDataAsset = CharacterDataAsset.LoadSynchronous())
	{
		return LoadedCharacterDataAsset->MovementParams.MaxWalkSpeedCrouchedSlow;
	}
	return 0.0f;
}

float AShooterCharacter::GetMaxWalkSpeedProned() const
{
	if (const UCharacterDataAsset* LoadedCharacterDataAsset = CharacterDataAsset.LoadSynchronous())
	{
		return LoadedCharacterDataAsset->MovementParams.MaxWalkSpeedProned;
	}
	return 0.0f;
}

float AShooterCharacter::GetMaxWalkSpeedSlow() const
{
	if (const UCharacterDataAsset* LoadedCharacterDataAsset = CharacterDataAsset.LoadSynchronous())
	{
		return LoadedCharacterDataAsset->MovementParams.MaxWalkSpeedSlow;
	}
	return 0.0f;
}

float AShooterCharacter::GetMaxWalkSpeedSprint() const
{
	if (const UCharacterDataAsset* LoadedCharacterDataAsset = CharacterDataAsset.LoadSynchronous())
	{
		return LoadedCharacterDataAsset->MovementParams.MaxWalkSpeedSprint;
	}
	return 0.0f;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceTurnInPlace()
{
	TurnDirection = ETurnDirection::TD_None;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceControllerDesiredRotationNeeded(bool bControllerDesiredRotationNeeded)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = bControllerDesiredRotationNeeded;
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
	if (CombatComponent && !(bIsToggleSprint && GetVelocity().SizeSquared2D() > 0.0f && bIsMoveInputForward))
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
	if (CurrentValue && CanPerformCrouchAction())
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
		Server_SetCurrentStance(NewStance);

		if (bIsToggleSprint)
		{
			Server_SetIsToggleSprint(false);
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
	if (CurrentValue && !(bIsToggleSprint && GetVelocity().SizeSquared2D() > 0.0f && bIsMoveInputForward))
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
		Server_SetLeanDirection(NewLeanDirection);
		Server_SetLeanTransitionDuration(TransitionDuration);
		Server_SetLeaningRate(MaxLeanRotation / TransitionDuration);
	}
}

void AShooterCharacter::OnCharacterLeanRightAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue && !(bIsToggleSprint && GetVelocity().SizeSquared2D() > 0.0f && bIsMoveInputForward))
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
		Server_SetLeanDirection(NewLeanDirection);
		Server_SetLeanTransitionDuration(TransitionDuration);
		Server_SetLeaningRate(MaxLeanRotation / TransitionDuration);
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
	float CurrentValue = Value.Get<float>();
	if (CanPerformMoveAction())
	{
		bIsMoveInputBackward = CurrentValue != 0.0f;
		if (bIsMoveInputForward)
		{
			CurrentValue = 0.0f;
		}
		if (CurrentValue)
		{
			AddMovementInput(GetActorForwardVector(), CurrentValue);
		}
	}
}

void AShooterCharacter::OnCharacterMoveForwardAction(const FInputActionValue& Value)
{
	float CurrentValue = Value.Get<float>();
	if (CanPerformMoveAction())
	{
		bIsMoveInputForward = CurrentValue != 0.0f;
		if (bIsMoveInputBackward)
		{
			CurrentValue = 0.0f;
		}
		if (CurrentValue)
		{
			AddMovementInput(GetActorForwardVector(), CurrentValue);

			if (CurrentStance == ECharacterStance::CS_Crouch && bIsToggleSprint)
			{
				TransitionToSprint();
			}
		}
	}
}

void AShooterCharacter::OnCharacterMoveLeftAction(const FInputActionValue& Value)
{
	float CurrentValue = Value.Get<float>();
	if (CanPerformMoveAction())
	{
		bIsMoveInputLeft = CurrentValue != 0.0f;
		if (bIsMoveInputRight)
		{
			CurrentValue = 0.0f;
		}
		if (CurrentValue)
		{
			if (bIsMoveInputForward && bIsToggleSprint)
			{
				CurrentValue *= FMath::Tan(FMath::DegreesToRadians(20.0f));
			}
			AddMovementInput(GetActorRightVector(), CurrentValue);
		}
	}
}

void AShooterCharacter::OnCharacterMoveRightAction(const FInputActionValue& Value)
{
	float CurrentValue = Value.Get<float>();
	if (CanPerformMoveAction())
	{
		bIsMoveInputRight = CurrentValue != 0.0f;
		if (bIsMoveInputLeft)
		{
			CurrentValue = 0.0f;
		}
		if (CurrentValue)
		{
			if (bIsMoveInputForward && bIsToggleSprint)
			{
				CurrentValue *= FMath::Tan(FMath::DegreesToRadians(20.0f));
			}
			AddMovementInput(GetActorRightVector(), CurrentValue);
		}
	}
}

void AShooterCharacter::OnCharacterProneAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue && CanPerformProneAction())
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
		Server_SetCurrentStance(NewStance);

		if (bIsToggleSprint)
		{
			Server_SetIsToggleSprint(false);
		}
	}
}

void AShooterCharacter::OnCharacterSlowAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	Server_SetIsToggleSlow(CurrentValue);

	if (CurrentValue && bIsToggleSprint)
	{
		Server_SetIsToggleSprint(false);
	}
}

void AShooterCharacter::OnCharacterSprintAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	Server_SetIsToggleSprint(CurrentValue);

	if (CurrentValue && GetVelocity().SizeSquared2D() > 0.0f && bIsMoveInputForward)
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

void AShooterCharacter::OnRep_CurrentStance()
{
	UpdateMaxWalkSpeed(CurrentStance, bIsToggleSlow, bIsToggleSprint);
}

void AShooterCharacter::OnRep_IsToggleSlow()
{
	UpdateMaxWalkSpeed(CurrentStance, bIsToggleSlow, bIsToggleSprint);
}

void AShooterCharacter::OnRep_IsToggleSprint()
{
	UpdateMaxWalkSpeed(CurrentStance, bIsToggleSlow, bIsToggleSprint);
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
	
	UpdateMaxWalkSpeed(CurrentStance, bIsToggleSlow, bIsToggleSprint);
}

void AShooterCharacter::Server_SetIsAlterAction_Implementation(bool bAlterAction)
{
	bIsAlterAction = bAlterAction;
}

void AShooterCharacter::Server_SetIsToggleSlow_Implementation(bool bToggleSlow)
{
	bIsToggleSlow = bToggleSlow;

	UpdateMaxWalkSpeed(CurrentStance, bIsToggleSlow, bIsToggleSprint);
}

void AShooterCharacter::Server_SetIsToggleSprint_Implementation(bool bToggleSprint)
{
	bIsToggleSprint = bToggleSprint;

	UpdateMaxWalkSpeed(CurrentStance, bIsToggleSlow, bIsToggleSprint);
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

//void AShooterCharacter::SetIsRemoteAccelerating(bool bRemoteAccelerating)
//{
//	bIsRemoteAccelerating = bRemoteAccelerating;
//}

//void AShooterCharacter::SetRemoteAcceleration(FVector NewRemoteAcceleration)
//{
//	RemoteAcceleration = NewRemoteAcceleration;
//}

void AShooterCharacter::SetRemoteViewYaw(float NewRemoteYaw)
{
	RemoteViewYaw = FRotator::CompressAxisToByte(NewRemoteYaw);
}

void AShooterCharacter::TransitionToSprint()
{
	if (CurrentStance == ECharacterStance::CS_Crouch)
	{
		UnCrouch();
	}
	Server_SetCurrentStance(ECharacterStance::CS_Stand);

	if (GetIsAiming())
	{
		CombatComponent->Server_SetIsAiming(false);
	}

	if (LeanDirection != ELeanDirection::LD_None)
	{
		Server_SetLeanDirection(ELeanDirection::LD_None);
		Server_SetLeanTransitionDuration(DefaultAnimationTransitionDuration);
		Server_SetLeaningRate(MaxLean / DefaultAnimationTransitionDuration);
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

void AShooterCharacter::UpdateMaxWalkSpeed(ECharacterStance Stance, bool bToggleSlow, bool bToggleSprint)
{
	if (GetCharacterMovement() == nullptr)
	{
		return;
	}
	if (Stance == ECharacterStance::CS_Stand)
	{
		GetCharacterMovement()->MaxWalkSpeed = bToggleSlow ? GetMaxWalkSpeedSlow() : GetMaxWalkSpeed();
		if (bToggleSprint)
		{
			GetCharacterMovement()->MaxWalkSpeed = GetMaxWalkSpeedSprint();
		}
	}
	else if (Stance == ECharacterStance::CS_Crouch)
	{
		GetCharacterMovement()->MaxWalkSpeedCrouched = bToggleSlow ? GetMaxWalkSpeedCrouchedSlow() : GetMaxWalkSpeedCrouched();
	}
	else if (Stance == ECharacterStance::CS_Prone)
	{
		GetCharacterMovement()->MaxWalkSpeed = GetMaxWalkSpeedProned();
	}

}

//void AShooterCharacter::UpdateMovement(float DeltaTime)
//{
//	float bMovementInput = MovementInputVector.Size() > 0.0 ? true : false;
//	float CurrentYaw = UKismetMathLibrary::NormalizedDeltaRotator(GetControlRotation(), GetActorRotation()).Yaw;
//	if (Controller == nullptr)
//	{
//		CurrentYaw = RemoteViewYaw;
//	}
//
//	if (FMath::IsNearlyZero(CurrentYaw, UE_KINDA_SMALL_NUMBER) || bMovementInput)
//	{
//		TurnDirection = ETurnDirection::TD_None;
//	}
//	else if (CurrentYaw < -90.0f)
//	{
//		TurnDirection = ETurnDirection::TD_Left;
//	}
//	else if (CurrentYaw > 90.0f)
//	{
//		TurnDirection = ETurnDirection::TD_Right;
//	}
//
//	if (TurnDirection != ETurnDirection::TD_None || bMovementInput)
//	{	
//		GetCharacterMovement()->bUseControllerDesiredRotation = true;
//
//		float ExceedingYaw = 0.0f;
//		if (CurrentYaw < -90.0f)
//		{
//			ExceedingYaw = CurrentYaw + 90.0f;
//		}
//		else if (CurrentYaw > 90.0f)
//		{
//			ExceedingYaw = CurrentYaw - 90.0f;
//		}
//
//		if (bMovementInput)
//		{
//			float YawRotationStep = GetCharacterMovement()->RotationRate.Yaw * DeltaTime;
//			if (FMath::Abs(CurrentYaw) < YawRotationStep)
//			{
//				YawRotationStep = CurrentYaw;
//			}
//			else if (CurrentYaw < 0.0)
//			{
//				YawRotationStep *= -1.0f;
//			}
//			ExceedingYaw += YawRotationStep;
//		}
//
//		AddActorLocalRotation(FRotator(0.0, ExceedingYaw, 0.0));
//		CurrentYaw -= ExceedingYaw;
//	}
//	else
//	{
//		GetCharacterMovement()->bUseControllerDesiredRotation = false;
//	}
//
//	if (IsLocallyControlled())
//	{
//		AO_Yaw = CurrentYaw;
//	}
//	else
//	{
//		AO_Yaw = FMath::FInterpTo(AO_Yaw, CurrentYaw, DeltaTime, 15.0f);
//	}
//	
//	if (IsLocallyControlled())
//	{
//		SetRemoteViewYaw(AO_Yaw);
//		Server_SetTurnDirection(TurnDirection);
//	}
//	
//}
