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
#include "Actor/Mag.h"
#include "Actor/Weapon.h"
#include "ActorComponent/CombatComponent.h"
#include "ActorComponent/InventoryComponent.h"
#include "AnimInstance/CharacterAnimInstance.h"
#include "AnimInstance/HandsAnimInstance.h"
#include "CharacterMovementComponent/ShooterCharacterMovementComponent.h"
#include "DataAsset/CharacterDataAsset.h"
#include "Enum/LeanDirection.h"
#include "Struct/ShooterUtility.h"
#include "Type/ShooterNameType.h"

AShooterCharacter::AShooterCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UShooterCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

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

	ShooterCharacterMovementComponent = Cast<UShooterCharacterMovementComponent>(GetCharacterMovement());

	RecalculatePronedEyeHeight();

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->RotationRate.Yaw = 720.0f;

	ADSTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ADSTimeline"));
	OnADSTimelineUpdate.BindUFunction(this, TEXT("Handle_ADSTimelineUpdate"));
	//OnADSTimelineFinished.BindUFunction(this, TEXT("Handle_ADSTimelineFinished"));

}

void AShooterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (USkeletalMeshComponent* CharacterMesh = GetMesh())
	{
		CharacterMesh->SetSkeletalMeshAsset(USkeletalMergingLibrary::MergeMeshes(CharacterMeshMergeParams));
		if (UCharacterAnimInstance* CharacterAnimInstance = Cast<UCharacterAnimInstance>(CharacterMesh->GetAnimInstance()))
		{
			CharacterAnimInstance->OnCharacterAnimInstanceTurningInPlace.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceTurningInPlace);
			CharacterAnimInstance->OnCharacterAnimInstanceCrouchAimToTransitionIdleLowAimToProneIdleAimStarted.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceCrouchAimToTransitionIdleLowAimToProneIdleAimStarted);
			CharacterAnimInstance->OnCharacterAnimInstanceCrouchAimSlowToTransitionIdleLowAimToProneIdleAimStarted.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceCrouchAimSlowToTransitionIdleLowAimToProneIdleAimStarted);
			CharacterAnimInstance->OnCharacterAnimInstanceIdleAimToTransitionIdleAimToProneIdleAimStarted.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceIdleAimToTransitionIdleAimToProneIdleAimStarted);
			CharacterAnimInstance->OnCharacterAnimInstanceIdleAimToTransitionIdleAimToSprintSlowStarted.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceIdleAimToTransitionIdleAimToSprintSlowStarted);
			CharacterAnimInstance->OnCharacterAnimInstanceIdleLowAimToTransitionIdleLowAimToProneIdleAimStarted.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceIdleLowAimToTransitionIdleLowAimToProneIdleAimStarted);
			CharacterAnimInstance->OnCharacterAnimInstanceProneFastToTransitionProneIdleAimToIdleAimStarted.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceProneFastToTransitionProneIdleAimToIdleAimStarted);
			CharacterAnimInstance->OnCharacterAnimInstanceProneFastToTransitionProneIdleAimToIdleLowAimStarted.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceProneFastToTransitionProneIdleAimToIdleLowAimStarted);
			CharacterAnimInstance->OnCharacterAnimInstanceProneIdleAimToTransitionProneIdleAimToIdleAimStarted.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceProneIdleAimToTransitionProneIdleAimToIdleAimStarted);
			CharacterAnimInstance->OnCharacterAnimInstanceProneIdleAimToTransitionProneIdleAimToIdleLowAimStarted.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceProneIdleAimToTransitionProneIdleAimToIdleLowAimStarted);
			CharacterAnimInstance->OnCharacterAnimInstanceSprintSlowToTransitionSprintSlowToCrouchIdleAimStarted.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceSprintSlowToTransitionSprintSlowToCrouchIdleAimStarted);
			CharacterAnimInstance->OnCharacterAnimInstanceSprintSlowToTransitionSprintSlowToIdleAimStarted.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceSprintSlowToTransitionSprintSlowToIdleAimStarted);
			CharacterAnimInstance->OnCharacterAnimInstanceSprintSlowToTransitionSprintSlowToProneIdleAimStarted.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceSprintSlowToTransitionSprintSlowToProneIdleAimStarted);
			CharacterAnimInstance->OnCharacterAnimInstanceTransitionIdleAimToProneIdleAimToProneIdleAimStarted.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceTransitionIdleAimToProneIdleAimToProneIdleAimStarted);
			CharacterAnimInstance->OnCharacterAnimInstanceTransitionIdleAimToSprintSlowToSprintSlowStarted.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceTransitionIdleAimToSprintSlowToSprintSlowStarted);
			CharacterAnimInstance->OnCharacterAnimInstanceTransitionIdleLowAimToProneIdleAimToProneIdleAimStarted.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceTransitionIdleLowAimToProneIdleAimToProneIdleAimStarted);
			CharacterAnimInstance->OnCharacterAnimInstanceTransitionProneIdleAimToIdleAimToIdleAimStarted.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceTransitionProneIdleAimToIdleAimToIdleAimStarted);
			CharacterAnimInstance->OnCharacterAnimInstanceTransitionProneIdleAimToIdleLowAimToIdleLowAimStarted.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceTransitionProneIdleAimToIdleLowAimToIdleLowAimStarted);
			CharacterAnimInstance->OnCharacterAnimInstanceTransitionSprintSlowToCrouchIdleAimToIdleLowAimStarted.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceTransitionSprintSlowToCrouchIdleAimToIdleLowAimStarted);
			CharacterAnimInstance->OnCharacterAnimInstanceTransitionSprintSlowToIdleAimToIdleAimStarted.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceTransitionSprintSlowToIdleAimToIdleAimStarted);
			CharacterAnimInstance->OnCharacterAnimInstanceTransitionSprintSlowToIdleAimToIdleLowAimStarted.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceTransitionSprintSlowToIdleAimToIdleLowAimStarted);
			CharacterAnimInstance->OnCharacterAnimInstanceTransitionSprintSlowToProneIdleAimToProneIdleAimStarted.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceTransitionSprintSlowToProneIdleAimToProneIdleAimStarted);
			CharacterAnimInstance->OnCharacterAnimInstanceWalkAimToTransitionIdleAimToProneIdleAimStarted.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceWalkAimToTransitionIdleAimToProneIdleAimStarted);
			CharacterAnimInstance->OnCharacterAnimInstanceWalkAimSlowToTransitionIdleAimToProneIdleAimStarted.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceWalkAimSlowToTransitionIdleAimToProneIdleAimStarted);
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

	if (ADSTimeline)
	{
		ADSTimeline->AddInterpFloat(ADSTimelineCurve.LoadSynchronous(), OnADSTimelineUpdate);
		//ADSTimeline->SetTimelineFinishedFunc(OnADSTimelineFinished);
		ADSTimeline->SetLooping(false);
		ADSTimeline->SetIgnoreTimeDilation(true);
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

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterCharacter, LeanDirection);
	DOREPLIFETIME_CONDITION(AShooterCharacter, TurningDirection, COND_SkipOwner);
	DOREPLIFETIME(AShooterCharacter, LeaningRate);
	DOREPLIFETIME(AShooterCharacter, LeanTransitionDuration);
	DOREPLIFETIME_CONDITION(AShooterCharacter, bIsProned, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(AShooterCharacter, bIsSlowing, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(AShooterCharacter, bIsSprinting, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(AShooterCharacter, bIsTransition, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(AShooterCharacter, RemoteViewYaw, COND_SkipOwner);

}

void AShooterCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	if (HasAuthority())
	{
		if (GetController())
		{
			SetRemoteViewYaw(GetController()->GetControlRotation().Yaw);
		}
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

		for (uint8 WeaponIndex = 0; WeaponIndex < InventoryComponent->GetWeaponArray().Num(); ++WeaponIndex)
		{
			InventoryComponent->LoadAmmoInWeaponMag(WeaponIndex);
		}

		for (AWeapon* Weapon : InventoryComponent->GetWeaponArray())
		{
			FName WeaponHolsterSocketName = GetCharacterWeaponHolsterSocketName(Weapon);
			Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, WeaponHolsterSocketName);
			Weapon->bIsHolster = true;
		}
	}

}

bool AShooterCharacter::CanProne() const
{
	return !bIsProned && GetRootComponent() && !GetRootComponent()->IsSimulatingPhysics();
}

bool AShooterCharacter::CanSlow() const
{
	return !bIsSlowing && !bIsProned && GetRootComponent() && !GetRootComponent()->IsSimulatingPhysics();
}

bool AShooterCharacter::CanSprint() const
{
	return !bIsSprinting && !bIsSlowing && GetRootComponent() && !GetRootComponent()->IsSimulatingPhysics();
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

float AShooterCharacter::GetAO_Yaw(float CurrentYaw, float DeltaTime) const
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

FVector AShooterCharacter::GetCurrentAcceleration() const
{
	return GetCharacterMovement() ? GetCharacterMovement()->GetCurrentAcceleration() : FVector::ZeroVector;
}

void AShooterCharacter::OnEndProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	RecalculateBaseEyeHeight();

	const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
	if (GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z;
	}
}

void AShooterCharacter::OnRep_IsProned()
{
	if (ShooterCharacterMovementComponent)
	{
		if (bIsProned)
		{
			ShooterCharacterMovementComponent->bWantsToProne = true;
			ShooterCharacterMovementComponent->Prone(true);
		}
		else
		{
			ShooterCharacterMovementComponent->bWantsToProne = false;
			ShooterCharacterMovementComponent->UnProne(true);
			if (bIsCrouched)
			{
				ShooterCharacterMovementComponent->Crouch(true);
				ShooterCharacterMovementComponent->bWantsToCrouch = true;
			}
		}
		ShooterCharacterMovementComponent->bNetworkUpdateReceived = true;
	}
}

void AShooterCharacter::OnStartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	RecalculateBaseEyeHeight();

	const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
	if (GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z + HalfHeightAdjust;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z + HalfHeightAdjust;
	}
}

void AShooterCharacter::Prone(bool bClientSimulation)
{
	if (ShooterCharacterMovementComponent && CanProne())
	{
		ShooterCharacterMovementComponent->bWantsToProne = true;
	}
}

void AShooterCharacter::RecalculateBaseEyeHeight()
{
	if (!bIsProned)
	{
		Super::RecalculateBaseEyeHeight();
	}
	else
	{
		BaseEyeHeight = PronedEyeHeight;
	}
}

void AShooterCharacter::RecalculatePronedEyeHeight()
{
	if (ShooterCharacterMovementComponent != nullptr)
	{
		constexpr float EyeHeightRatio = 0.8f;	// how high the character's eyes are, relative to the proned height

		PronedEyeHeight = ShooterCharacterMovementComponent->GetPronedHalfHeight() * EyeHeightRatio;
	}
}

void AShooterCharacter::Slow()
{
	if (ShooterCharacterMovementComponent && CanSlow())
	{
		ShooterCharacterMovementComponent->bWantsToSlow = true;
	}
}

void AShooterCharacter::Sprint()
{
	if (ShooterCharacterMovementComponent && CanSprint())
	{
		ShooterCharacterMovementComponent->bWantsToSprint = true;
	}
}

void AShooterCharacter::UnProne(bool bClientSimulation)
{
	if (ShooterCharacterMovementComponent)
	{
		ShooterCharacterMovementComponent->bWantsToProne = false;
	}
}

void AShooterCharacter::UnSlow()
{
	if (ShooterCharacterMovementComponent)
	{
		ShooterCharacterMovementComponent->bWantsToSlow = false;
	}
}

void AShooterCharacter::UnSprint()
{
	if (ShooterCharacterMovementComponent)
	{
		ShooterCharacterMovementComponent->bWantsToSprint = false;
	}
}

//void AShooterCharacter::SetIsTransition(bool bNewIsTransition)
//{
//	if (GetController() && GetController()->IsLocalController())
//	{
//		GetController()->SetIgnoreMoveInput(bNewIsTransition);
//	}
//	bIsTransition = bNewIsTransition;
//}

bool AShooterCharacter::CanAim() const
{
	return !bIsAiming && !IsCrawling();
}

void AShooterCharacter::Aim()
{
	if (CanAim())
	{
		if (bIsSprinting)
		{
			UnSprint();
		}

		bIsAiming = true;
		UpdateADSCameraTargetLocation();
		ADSTimeline->Play();
	}
}

void AShooterCharacter::UnAim()
{
	bIsAiming = false;
	ADSTimeline->Reverse();
}

bool AShooterCharacter::IsCrawling() const
{
	return bIsProned && GetVelocity().SizeSquared2D() > 0.0f && GetCurrentAcceleration().SizeSquared2D() > 0.0f;
}

bool AShooterCharacter::CanPerformCrouchAction() const
{
	//return TurnDirection == ETurnDirection::TD_None && bIsTransition == false;
	return bIsTransition == false;
}

bool AShooterCharacter::CanPerformMoveAction() const
{
	return bIsTransition == false;
}

bool AShooterCharacter::CanPerformProneAction() const
{
	//return TurnDirection == ETurnDirection::TD_None && bIsTransition == false;
	return bIsTransition == false;
}

void AShooterCharacter::Handle_ADSTimelineUpdate(float Value)
{
	if (FirstPersonCamera)
	{
		float DeltaFOV = (DefaultCameraFOV * 0.8f) - DefaultCameraFOV;
		FirstPersonCamera->SetRelativeLocation(ADSCameraTargetLocation * Value);
		FirstPersonCamera->SetFieldOfView(DefaultCameraFOV + (DeltaFOV * Value));
	}
}

//void AShooterCharacter::Handle_ADSTimelineFinished()
//{
//
//}

void AShooterCharacter::Handle_OnCharacterAnimInstanceTurningInPlace(ETurningDirection NewTurningDirection)
{
	TurningDirection = NewTurningDirection;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceCrouchAimToTransitionIdleLowAimToProneIdleAimStarted()
{
	bIsTransition = true;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceCrouchAimSlowToTransitionIdleLowAimToProneIdleAimStarted()
{
	bIsTransition = true;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceIdleAimToTransitionIdleAimToProneIdleAimStarted()
{
	bIsTransition = true;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceIdleAimToTransitionIdleAimToSprintSlowStarted()
{
	bIsTransition = true;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceIdleLowAimToTransitionIdleLowAimToProneIdleAimStarted()
{
	bIsTransition = true;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceProneFastToTransitionProneIdleAimToIdleAimStarted()
{
	bIsTransition = true;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceProneFastToTransitionProneIdleAimToIdleLowAimStarted()
{
	bIsTransition = true;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceProneIdleAimToTransitionProneIdleAimToIdleAimStarted()
{
	bIsTransition = true;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceProneIdleAimToTransitionProneIdleAimToIdleLowAimStarted()
{
	bIsTransition = true;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceSprintSlowToTransitionSprintSlowToCrouchIdleAimStarted()
{
	bIsTransition = true;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceSprintSlowToTransitionSprintSlowToIdleAimStarted()
{
	bIsTransition = true;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceSprintSlowToTransitionSprintSlowToProneIdleAimStarted()
{
	bIsTransition = true;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceTransitionIdleAimToProneIdleAimToProneIdleAimStarted()
{
	bIsTransition = false;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceTransitionIdleAimToSprintSlowToSprintSlowStarted()
{
	bIsTransition = false;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceTransitionIdleLowAimToProneIdleAimToProneIdleAimStarted()
{
	bIsTransition = false;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceTransitionProneIdleAimToIdleAimToIdleAimStarted()
{
	bIsTransition = false;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceTransitionProneIdleAimToIdleLowAimToIdleLowAimStarted()
{
	bIsTransition = false;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceTransitionSprintSlowToCrouchIdleAimToIdleLowAimStarted()
{
	bIsTransition = false;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceTransitionSprintSlowToIdleAimToIdleAimStarted()
{
	bIsTransition = false;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceTransitionSprintSlowToIdleAimToIdleLowAimStarted()
{
	bIsTransition = false;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceTransitionSprintSlowToProneIdleAimToProneIdleAimStarted()
{
	bIsTransition = false;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceWalkAimToTransitionIdleAimToProneIdleAimStarted()
{
	bIsTransition = true;
}

void AShooterCharacter::Handle_OnCharacterAnimInstanceWalkAimSlowToTransitionIdleAimToProneIdleAimStarted()
{
	bIsTransition = true;
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
		CombatComponent->Out();
	}
}

void AShooterCharacter::Handle_OnCombatComponentOut(AWeapon* Weapon)
{
	if (HasAuthority() && CombatComponent)
	{
		if (AWeapon* EquippedWeapon = CombatComponent->GetEquippedWeapon())
		{
			FName WeaponHolsterSocketName = GetCharacterWeaponHolsterSocketName(EquippedWeapon);
			CombatComponent->UnequipWeapon(GetMesh(), WeaponHolsterSocketName);
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
		CombatComponent->Idle();
	}
}

void AShooterCharacter::Handle_OnCombatComponentOutToIdleArm(AWeapon* Weapon)
{
	if (HasAuthority() && CombatComponent)
	{
		CombatComponent->Idle();
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
		CombatComponent->Idle();
	}
}

void AShooterCharacter::Handle_OnCombatComponentWeaponFiremodeCheck(AWeapon* Weapon)
{
	if (HasAuthority() && CombatComponent)
	{
		CombatComponent->Idle();
	}
}

void AShooterCharacter::Handle_OnCombatComponentWeaponMagCheck(AWeapon* Weapon)
{
}

void AShooterCharacter::Handle_OnCombatComponentWeaponMagIn(AWeapon* Weapon)
{
	if (HasAuthority() && Weapon && CombatComponent)
	{
		const AMag* WeaponMag = Weapon ? Weapon->GetMag() : nullptr;
		const uint8 MagAmmoCount = WeaponMag ? WeaponMag->GetAmmoCount() : 0;
		const bool bHasPatronInWeaponAmmo = Weapon && Weapon->GetPatronInWeaponAmmo() != nullptr;
		if (WeaponMag && MagAmmoCount > 0 && !bHasPatronInWeaponAmmo)
		{
			CombatComponent->WeaponReloadCharge();
		}
		else
		{
			CombatComponent->ActionEnd();
		}
	}

}

void AShooterCharacter::Handle_OnCombatComponentWeaponMagOut(AWeapon* Weapon)
{
	if (HasAuthority() && Weapon && CombatComponent && InventoryComponent)
	{
		int8 WeaponIndex = InventoryComponent->FindWeapon(Weapon);
		if (WeaponIndex != INDEX_NONE)
		{
			InventoryComponent->LoadAmmoInWeaponMag(WeaponIndex);
		}
		CombatComponent->WeaponMagIn();
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
		CombatComponent->Out();
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
		CombatComponent->Idle();
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
	if (CurrentValue)
	{
		Aim();
	}
	else
	{
		UnAim();
	}
}

void AShooterCharacter::OnCharacterAlterAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	bIsAlterAction = CurrentValue;
}

void AShooterCharacter::OnCharacterCrouchAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue && CanPerformCrouchAction())
	{
		if (!bIsCrouched)
		{
			Crouch();
		}
		else
		{
			UnCrouch();
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
	bool bIsToggleSprint = false;	// temp
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
	bool bIsToggleSprint = false;	// temp
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

			if (bIsProned && bIsAiming)
			{
				UnAim();
			}
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

			if (bIsProned && bIsAiming)
			{
				UnAim();
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
			if (bIsSprinting)
			{
				CurrentValue *= FMath::Tan(FMath::DegreesToRadians(20.0f));
			}
			AddMovementInput(GetActorRightVector(), CurrentValue);

			if (bIsProned && bIsAiming)
			{
				UnAim();
			}
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
			if (bIsSprinting)
			{
				CurrentValue *= FMath::Tan(FMath::DegreesToRadians(20.0f));
			}
			AddMovementInput(GetActorRightVector(), CurrentValue);

			if (bIsProned && bIsAiming)
			{
				UnAim();
			}
		}
	}
}

void AShooterCharacter::OnCharacterProneAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue && CanPerformProneAction())
	{
		if (!bIsProned)
		{
			Prone();
		}
		else
		{
			UnProne();
		}
	}
}

void AShooterCharacter::OnCharacterSlowAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		Slow();
	}
	else
	{
		UnSlow();
	}
}

void AShooterCharacter::OnCharacterSprintAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		Sprint();
	}
	else
	{
		UnSprint();
	}
}

void AShooterCharacter::OnWeaponChamberCheckAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue && bIsAlterAction)
	{
		Server_CheckWeaponChamber();
	}
}

void AShooterCharacter::OnWeaponFireAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	Server_FireWeapon(CurrentValue);
}

void AShooterCharacter::OnWeaponFiremodeAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		if (bIsAlterAction)
		{
			Server_CheckWeaponFiremode();
		}
		else
		{
			Server_ChangeWeaponFiremode();
		}
	}
}

void AShooterCharacter::OnWeaponReloadAction(const FInputActionValue& Value)
{
	const bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		if (bIsAlterAction)
		{
			Server_CheckWeaponMag();
		}
		else
		{
			Server_WeaponMagOut();
		}
	}
}

void AShooterCharacter::Server_EquipWeaponProgressive_Implementation(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr || CombatComponent == nullptr)
	{
		return;
	}
	const ECombatAction CombatAction = CombatComponent->GetCombatAction();
	if (NextWeaponToEquip == nullptr && CombatAction == ECombatAction::CA_Idle)
	{
		NextWeaponToEquip = WeaponToEquip;
		CombatComponent->IdleToOut();
	}
	else if (NextWeaponToEquip && CombatAction == ECombatAction::CA_Out)
	{
		CombatComponent->EquipWeapon(NextWeaponToEquip, HandsMesh, BASE_HUMAN_RIBCAGE_SOCKET_NAME);
		CombatComponent->OutToIdle();
		NextWeaponToEquip = nullptr;
	}
}

void AShooterCharacter::Server_HolsterWeaponProgressive_Implementation()
{
	if (CombatComponent == nullptr)
	{
		return;
	}
	const ECombatAction CombatAction = CombatComponent->GetCombatAction();
	if (CombatAction == ECombatAction::CA_Idle)
	{
		NextWeaponToEquip = nullptr;
		CombatComponent->IdleToOut();
	}
	else if (NextWeaponToEquip == nullptr && CombatAction == ECombatAction::CA_Out)
	{
		CombatComponent->OutToIdle();
	}
}

void AShooterCharacter::Server_CheckWeaponChamber_Implementation()
{
	if (CombatComponent)
	{
		CombatComponent->WeaponChamberCheck();
	}
}

void AShooterCharacter::Server_FireWeapon_Implementation(bool bFire)
{
	if (CombatComponent)
	{
		CombatComponent->WeaponFire(bFire);
	}
}

void AShooterCharacter::Server_ChangeWeaponFiremode_Implementation()
{
	if (CombatComponent)
	{
		CombatComponent->WeaponFiremode();
	}
}

void AShooterCharacter::Server_CheckWeaponFiremode_Implementation()
{
	if (CombatComponent)
	{
		CombatComponent->WeaponFiremodeCheck();
	}
}

void AShooterCharacter::Server_CheckWeaponMag_Implementation()
{
	if (CombatComponent)
	{
		CombatComponent->WeaponMagCheck();
	}
}

void AShooterCharacter::Server_WeaponMagOut_Implementation()
{
	if (CombatComponent && InventoryComponent)
	{
		if (AWeapon* EquippedWeapon = CombatComponent->GetEquippedWeapon())
		{
			const int8 WeaponIndex = InventoryComponent->FindWeapon(EquippedWeapon);
			if (WeaponIndex != INDEX_NONE)
			{
				const AMag* WeaponMag = EquippedWeapon->GetMag();
				const uint8 MagAmmoCount = WeaponMag ? WeaponMag->GetAmmoCount() : 0;
				const uint8 MagAmmoCapacity = WeaponMag ? WeaponMag->GetAmmoCapacity() : 0;
				const uint8 WeaponAmmoInInventory = InventoryComponent->GetWeaponAmmoAtIndex(WeaponIndex);
				if (WeaponAmmoInInventory > 0 && (MagAmmoCapacity - MagAmmoCount) > 0)
				{
					CombatComponent->WeaponMagOut();
				}
			}
		}
	}
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

void AShooterCharacter::SetRemoteViewYaw(float NewRemoteYaw)
{
	RemoteViewYaw = FRotator::CompressAxisToByte(NewRemoteYaw);
}

void AShooterCharacter::UpdateADSCameraTargetLocation()
{
	FTransform AimCameraTransform;
	if (AWeapon* EquippedWeapon = CombatComponent ? CombatComponent->GetEquippedWeapon() : nullptr)
	{
		if (USkeletalMeshComponent* WeaponScopeSightMesh = EquippedWeapon->GetScopeSightMesh())
		{
			AimCameraTransform = WeaponScopeSightMesh->GetSocketTransform(MOD_AIM_CAMERA_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		}
		else if (USkeletalMeshComponent* WeaponMesh = EquippedWeapon->GetMesh())
		{
			AimCameraTransform = WeaponMesh->GetSocketTransform(AIM_CAMERA_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		}
	}
	else if (HandsMesh)
	{
		AimCameraTransform = HandsMesh->GetSocketTransform(AIM_CAMERA_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
	}

	if (HandsMesh)
	{
		ADSCameraTargetLocation = FShooterUtility::TransformToBoneSpace(HandsMesh, CAMERA_ANIMATED_SOCKET_NAME, AimCameraTransform).GetLocation();
	}
}
