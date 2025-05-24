// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Animation/CharacterAnimInstance.h"
#include "Animation/HandsAnimInstance.h"
#include "Components/CharacterCombatComponent.h"
#include "Components/CharacterInventoryComponent.h"
#include "Data/CharacterDataAsset.h"
#include "GameFramework/ShooterCharacterMovementComponent.h"
#include "Mod/Mag.h"
#include "Types/CharacterTypes.h"
#include "Types/ShooterNames.h"
#include "Utility/ShooterUtility.h"
#include "Weapon/Weapon.h"

AShooterCharacter::AShooterCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UShooterCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	LegsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegsMesh"));
	LegsMesh->SetupAttachment(GetMesh());
	LegsMesh->bOnlyOwnerSee = true;
	LegsMesh->CastShadow = false;

	HandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandsMesh"));
	HandsMesh->SetupAttachment(GetMesh());
	//HandsMesh->bOnlyOwnerSee = true;
	HandsMesh->CastShadow = false;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(HandsMesh, CAMERA_ANIMATED_SOCKET_NAME);
	FirstPersonCamera->bConstrainAspectRatio = true;

	CharacterInventory = CreateDefaultSubobject<UCharacterInventoryComponent>(TEXT("CharacterInventory"));
	CharacterInventory->SetIsReplicated(true);

	CharacterCombat = CreateDefaultSubobject<UCharacterCombatComponent>(TEXT("CharacterCombatComponent"));
	CharacterCombat->SetIsReplicated(true);

	ShooterCharacterMovement = Cast<UShooterCharacterMovementComponent>(GetCharacterMovement());

	RecalculatePronedEyeHeight();

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	ADSTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ADSTimeline"));
	OnADSTimelineUpdate.BindUFunction(this, TEXT("Handle_OnADSTimelineUpdate"));
	//OnADSTimelineFinished.BindUFunction(this, TEXT("Handle_OnADSTimelineFinished"));

	IdleAnimTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("IdleAnimTimeline"));
	OnIdleAnimTimelineHorizontalMovementUpdate.BindUFunction(this, TEXT("Handle_OnIdleAnimTimelineHorizontalMovementUpdate"));
	OnIdleAnimTimelineVerticalMovementUpdate.BindUFunction(this, TEXT("Handle_OnIdleAnimTimelineVerticalMovementUpdate"));
	OnIdleAnimTimelineRollRotationUpdate.BindUFunction(this, TEXT("Handle_OnIdleAnimTimelineRollRotationUpdate"));

	WalkAnimTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("WalkAnimTimeline"));
	OnWalkAnimTimelineHorizontalMovementUpdate.BindUFunction(this, TEXT("Handle_OnWalkAnimTimelineHorizontalMovementUpdate"));
	OnWalkAnimTimelineVerticalMovementUpdate.BindUFunction(this, TEXT("Handle_OnWalkAnimTimelineVerticalMovementUpdate"));
	OnWalkAnimTimelineRollRotationUpdate.BindUFunction(this, TEXT("Handle_OnWalkAnimTimelineRollRotationUpdate"));

	TurningAnimTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("TurningAnimTimeline"));
	OnTurningAnimTimelineUpdate.BindUFunction(this, TEXT("Handle_OnTurningAnimTimelineUpdate"));
	OnTurningAnimTimelineFinished.BindUFunction(this, TEXT("Handle_OnTurningAnimTimelineFinished"));

	IdleAnimMaxHorizontalMovement = 0.15f;
	IdleAnimMinHorizontalMovement = -0.15f;
	IdleAnimMaxVerticalMovement = 0.25f;
	IdleAnimMinVerticalMovement = -0.5f;
	IdleAnimMaxRollRotation = 1.5f;
	IdleAnimMinRollRotation = -1.5f;

	WalkAnimMaxHorizontalMovement = 0.2f;
	WalkAnimMinHorizontalMovement = -0.2f;
	WalkAnimMaxVerticalMovement = 0.25f;
	WalkAnimMinVerticalMovement = -0.5f;
	WalkAnimMaxRollRotation = 2.5f;
	WalkAnimMinRollRotation = -2.5f;

	SwayHorizontalMovementLimit = 1.0f;
	SwayVerticalMovementLimit = 1.0f;
	SwayRollRotationLimit = 7.5f;

	SwayHorizontalMovementSensitivity = 0.025;
	SwayVerticalMovementSensitivity = 0.025;
	SwayRollRotationSensitivity = 0.075;

	LeaningMaxAngle = 15.0f;
	LeaningInterpSpeed = 5.0f;
	LeaningDefaultTransitionDuration = 0.25f;

}

void AShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterCharacter, LeaningDirection);
	DOREPLIFETIME_CONDITION(AShooterCharacter, bIsProned, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(AShooterCharacter, bIsSlowing, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(AShooterCharacter, bIsSprinting, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(AShooterCharacter, bIsTransition, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(AShooterCharacter, TurningDirection, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(AShooterCharacter, RootBoneYaw, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(AShooterCharacter, AO_Pitch, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(AShooterCharacter, AO_Yaw, COND_SimulatedOnly);

}

void AShooterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetMesh())
	{
		GetMesh()->SetSkeletalMeshAsset(USkeletalMergingLibrary::MergeMeshes(CharacterMeshMergeParams));
		if (UCharacterAnimInstance* CharacterAnimInstance = Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance()))
		{
			/*CharacterAnimInstance->OnCharacterAnimInstanceIdle.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceIdle);
			CharacterAnimInstance->OnCharacterAnimInstanceWalk.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceWalk);
			CharacterAnimInstance->OnCharacterAnimInstanceThirdOrCombatAction.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceThirdOrCombatAction);
			CharacterAnimInstance->OnCharacterAnimInstanceTurningInPlace.AddDynamic(this, &AShooterCharacter::Handle_OnCharacterAnimInstanceTurningInPlace);*/
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
	if (LegsMesh)
	{
		LegsMesh->SetLeaderPoseComponent(GetMesh());
	}
	if (FirstPersonCamera)
	{
		CameraDefaultFOV = FirstPersonCamera->FieldOfView;
	}

	if (CharacterCombat)
	{
		CharacterCombat->OnCombatComponentEquippedWeaponChanged.AddDynamic(this, &AShooterCharacter::Handle_OnCombatComponentEquippedWeaponChanged);
	}
	if (CharacterInventory)
	{
		CharacterInventory->OnInventoryComponentWeaponArrayReplicated.AddDynamic(this, &AShooterCharacter::Handle_OnInventoryComponentWeaponArrayReplicated);
	}
	if (ShooterCharacterMovement)
	{
		ShooterCharacterMovement->OnMovementComponentSprint.AddDynamic(this, &AShooterCharacter::Handle_OnMovementComponentSprint);
	}

	if (ADSTimeline)
	{
		ADSTimeline->AddInterpFloat(ADSTimelineCurve.LoadSynchronous(), OnADSTimelineUpdate);
		//ADSTimeline->SetTimelineFinishedFunc(OnADSTimelineFinished);
		ADSTimeline->SetLooping(false);
	}
	if (IdleAnimTimeline)
	{
		IdleAnimTimeline->AddInterpFloat(IdleAnimTimelineHorizontalMovementCurve.LoadSynchronous(), OnIdleAnimTimelineHorizontalMovementUpdate);
		IdleAnimTimeline->AddInterpFloat(IdleAnimTimelineVerticalMovementCurve.LoadSynchronous(), OnIdleAnimTimelineVerticalMovementUpdate);
		IdleAnimTimeline->AddInterpFloat(IdleAnimTimelineRollRotationCurve.LoadSynchronous(), OnIdleAnimTimelineRollRotationUpdate);
		IdleAnimTimeline->SetLooping(true);
	}
	if (WalkAnimTimeline)
	{
		WalkAnimTimeline->AddInterpFloat(WalkAnimTimelineHorizontalMovementCurve.LoadSynchronous(), OnWalkAnimTimelineHorizontalMovementUpdate);
		WalkAnimTimeline->AddInterpFloat(WalkAnimTimelineVerticalMovementCurve.LoadSynchronous(), OnWalkAnimTimelineVerticalMovementUpdate);
		WalkAnimTimeline->AddInterpFloat(WalkAnimTimelineRollRotationCurve.LoadSynchronous(), OnWalkAnimTimelineRollRotationUpdate);
		WalkAnimTimeline->SetLooping(true);
	}
	if (TurningAnimTimeline)
	{
		TurningAnimTimeline->AddInterpFloat(TurningAnimTimelineCurve.LoadSynchronous(), OnTurningAnimTimelineUpdate);
		TurningAnimTimeline->SetTimelineFinishedFunc(OnTurningAnimTimelineFinished);
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

//void AShooterCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
//{
//	Super::PreReplication(ChangedPropertyTracker);
//
//	/*if (HasAuthority())
//	{
//		if (GetController())
//		{
//			SetRemoteViewYaw(GetController()->GetControlRotation().Yaw);
//		}
//	}*/
//}

//void AShooterCharacter::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

void AShooterCharacter::Init()
{
	if (CharacterInventory)
	{
		if (CharacterDataAsset.LoadSynchronous())
		{
			CharacterInventory->Init(CharacterDataAsset->InventoryParams);
		}

		for (uint8 WeaponIndex = 0; WeaponIndex < CharacterInventory->GetWeaponArray().Num(); ++WeaponIndex)
		{
			CharacterInventory->LoadAmmoInWeaponMag(WeaponIndex);
		}

		for (AWeapon* Weapon : CharacterInventory->GetWeaponArray())
		{
			Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
			//Weapon->bIsHolster = true;
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
			EnhancedInputLocalPlayerSubsystem->AddMappingContext(OtherMappingContext.LoadSynchronous(), 0);
		}
	}

	if (IsLocallyControlled())
	{
		if (USkeletalMeshComponent* CharacterMesh = GetMesh())
		{
			CharacterMesh->SetVisibility(false);
			CharacterMesh->SetCastHiddenShadow(true);
		}

		if (CharacterInventory)
		{
			AWeapon* PrimaryWeapon = CharacterInventory->GetWeaponAtIndex(PRIMARY_WEAPON_INDEX);
			Server_EquipWeapon(PrimaryWeapon);
		}
	}

}

ECombatAction AShooterCharacter::GetCombatAction() const
{
	return CharacterCombat != nullptr ? CharacterCombat->GetCombatAction() : ECombatAction::CA_None;
}

ECombatAction AShooterCharacter::GetAnimCombatAction() const
{
	return CharacterCombat != nullptr ? CharacterCombat->GetAnimCombatAction() : ECombatAction::CA_None;
}

AWeapon* AShooterCharacter::GetEquippedWeapon() const
{
	return CharacterCombat != nullptr ? CharacterCombat->GetEquippedWeapon() : nullptr;
}

float AShooterCharacter::GetAllowedAO_Yaw() const
{
	return (!bIsCrouched && !bIsProned) || bIsCrouched ? 90.0f : 45.0f;
}

void AShooterCharacter::UpdateAO_Pitch(float ControlRotationPitch, float DeltaTime)
{
	AO_Pitch = ControlRotationPitch;
	if (AO_Pitch > 90.0f)
	{
		FVector2D InRange = FVector2D(270.0, 360.0);
		FVector2D OutRange = FVector2D(-90.0, 0.0);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void AShooterCharacter::UpdateAO_Yaw(float ControlRotationYaw, float DeltaTime)
{
	AO_Yaw = UKismetMathLibrary::NormalizedDeltaRotator(FRotator(0.0f, ControlRotationYaw, 0.0f), FRotator(0.0f, ReferenceActorRotationYaw, 0.0f)).Yaw;

	float AllowedAO_Yaw = GetAllowedAO_Yaw();
	float ExceedingAO_Yaw = AO_Yaw > AllowedAO_Yaw ? AO_Yaw - AllowedAO_Yaw : AO_Yaw < -AllowedAO_Yaw ? AO_Yaw + AllowedAO_Yaw : 0.0f;
	RootBoneYaw = ExceedingAO_Yaw - AO_Yaw;

	if (ExceedingAO_Yaw != 0.0f && TurningDirection == ETurningDirection::TD_None)
	{
		TurningDirection = ExceedingAO_Yaw > 0.0f ? ETurningDirection::TD_Right : ETurningDirection::TD_Left;
		if (TurningAnimTimeline && !TurningAnimTimeline->IsPlaying())
		{
			TurningAnimTimeline->PlayFromStart();
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

FName AShooterCharacter::GetCharacterWeaponHolsterSocketName(AWeapon* Weapon) const
{
	FName WeaponHolsterSocketName = NAME_None;
	if (Weapon && CharacterInventory)
	{
		if (Weapon->IsPistol())
		{
			WeaponHolsterSocketName = PISTOL_HOLSTER_SOCKET_NAME;
		}
		else
		{
			const int8 WeaponIndex = CharacterInventory->FindWeapon(Weapon);
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
	if (ShooterCharacterMovement)
	{
		if (bIsProned)
		{
			ShooterCharacterMovement->bWantsToProne = true;
			ShooterCharacterMovement->Prone(true);
		}
		else
		{
			ShooterCharacterMovement->bWantsToProne = false;
			ShooterCharacterMovement->UnProne(true);
			if (bIsCrouched)
			{
				ShooterCharacterMovement->Crouch(true);
				ShooterCharacterMovement->bWantsToCrouch = true;
			}
		}
		ShooterCharacterMovement->bNetworkUpdateReceived = true;
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
	if (ShooterCharacterMovement && CanProne())
	{
		ShooterCharacterMovement->bWantsToProne = true;
	}
}

void AShooterCharacter::RecalculatePronedEyeHeight()
{
	if (ShooterCharacterMovement!= nullptr)
	{
		constexpr float EyeHeightRatio = 0.8f;	// how high the character's eyes are, relative to the proned height

		PronedEyeHeight = ShooterCharacterMovement->GetPronedHalfHeight() * EyeHeightRatio;
	}
}

void AShooterCharacter::OnMovementUpdated(float DeltaTime, const FVector& OldLocation, const FVector& OldVelocity)
{
	FVector Velocity = GetVelocity();
	bool bHasVelocity = Velocity.SizeSquared2D() > 0.0f;
	FRotator ActorRotation = GetActorRotation();
	bool bIsThirdAction = bIsTransition || bIsSprinting || (bIsProned && bHasVelocity);
	
	if (bHasVelocity)
	{
		ReferenceActorRotationYaw = FMath::RInterpTo(FRotator(0.0f, ReferenceActorRotationYaw, 0.0f), FRotator(0.0f, ActorRotation.Yaw, 0.0f), DeltaTime, 15.0f).Yaw;

		float VelocityYaw = UKismetMathLibrary::MakeRotFromX(Velocity).Yaw;
		VelocityYawOffset = UKismetMathLibrary::NormalizedDeltaRotator(FRotator(0.0, VelocityYaw, 0.0), FRotator(0.0, ActorRotation.Yaw, 0.0)).Yaw;
	}

	if (GetCombatAction() == ECombatAction::CA_Idle && GetEquippedWeapon() && !bIsThirdAction)
	{
		if (bHasVelocity)
		{
			if (IdleAnimTimeline && IdleAnimTimeline->IsPlaying())
			{
				IdleAnimTimeline->Stop();
			}
			if (WalkAnimTimeline && !WalkAnimTimeline->IsPlaying())
			{
				WalkAnimTimeline->PlayFromStart();
			}
		}
		else
		{
			if (WalkAnimTimeline && WalkAnimTimeline->IsPlaying())
			{
				WalkAnimTimeline->Stop();
			}
			if (IdleAnimTimeline && !IdleAnimTimeline->IsPlaying())
			{
				IdleAnimTimeline->PlayFromStart();
			}
		}
	}
	else
	{
		if (IdleAnimTimeline && IdleAnimTimeline->IsPlaying())
		{
			IdleAnimTimeline->Stop();
		}
		if (WalkAnimTimeline && WalkAnimTimeline->IsPlaying())
		{
			WalkAnimTimeline->Stop();
		}
	}

}

void AShooterCharacter::Slow()
{
	if (ShooterCharacterMovement && CanSlow())
	{
		ShooterCharacterMovement->bWantsToSlow = true;
	}
}

void AShooterCharacter::Sprint()
{
	if (ShooterCharacterMovement && CanSprint())
	{
		ShooterCharacterMovement->bWantsToSprint = true;
	}
}

void AShooterCharacter::UnProne(bool bClientSimulation)
{
	if (ShooterCharacterMovement)
	{
		ShooterCharacterMovement->bWantsToProne = false;
	}
}

void AShooterCharacter::UnSlow()
{
	if (ShooterCharacterMovement)
	{
		ShooterCharacterMovement->bWantsToSlow = false;
	}
}

void AShooterCharacter::UnSprint()
{
	if (ShooterCharacterMovement)
	{
		ShooterCharacterMovement->bWantsToSprint = false;
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
	bool bIsAccelerating = GetCharacterMovement() && GetCharacterMovement()->GetCurrentAcceleration().SizeSquared2D() > 0.0f;
	return !bIsAiming && !(bIsProned && GetVelocity().SizeSquared2D() > 0.0f && bIsAccelerating);
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
		CalculateADSCameraTargetLocation();
		ADSTimeline->Play();
	}
}

void AShooterCharacter::UnAim()
{
	bIsAiming = false;
	ADSTimeline->Reverse();
}

bool AShooterCharacter::CanLean() const
{
	bool bIsAccelerating = GetCharacterMovement() && GetCharacterMovement()->GetCurrentAcceleration().SizeSquared2D() > 0.0f;
	bool bHasVelocity = GetVelocity().SizeSquared2D() > 0.0f;
	return !bIsSprinting && !(bIsProned && bHasVelocity && bIsAccelerating);
}

void AShooterCharacter::Server_Lean_Implementation(ELeaningDirection NewLeaningDirection)
{
	if (CanLean())
	{
		ELeaningDirection OldLeaningDirection = LeaningDirection;
		LeaningDirection = NewLeaningDirection;

		CalculateLeaningTargetAngle();
		CalculateLeaningTransitionDuration(OldLeaningDirection);
	}
}

void AShooterCharacter::Handle_OnADSTimelineUpdate(float Value)
{
	if (FirstPersonCamera)
	{
		float DeltaFOV = (CameraDefaultFOV * 0.8f) - CameraDefaultFOV;
		FirstPersonCamera->SetRelativeLocation(ADSCameraTargetLocation * Value);
		FirstPersonCamera->SetFieldOfView(CameraDefaultFOV + (DeltaFOV * Value));
	}
}

//void AShooterCharacter::Handle_OnADSTimelineFinished()
//{
//
//}

void AShooterCharacter::Handle_OnIdleAnimTimelineHorizontalMovementUpdate(float Value)
{
	ProceduralAnimHorizontalMovement = UKismetMathLibrary::Lerp(IdleAnimMinHorizontalMovement, IdleAnimMaxHorizontalMovement, Value);
}

void AShooterCharacter::Handle_OnIdleAnimTimelineVerticalMovementUpdate(float Value)
{
	ProceduralAnimVerticalMovement = UKismetMathLibrary::Lerp(IdleAnimMinVerticalMovement, IdleAnimMaxVerticalMovement, Value);
}

void AShooterCharacter::Handle_OnIdleAnimTimelineRollRotationUpdate(float Value)
{
	ProceduralAnimRollRotation = UKismetMathLibrary::Lerp(IdleAnimMinRollRotation, IdleAnimMaxRollRotation, Value);
}

void AShooterCharacter::Handle_OnWalkAnimTimelineHorizontalMovementUpdate(float Value)
{
	ProceduralAnimHorizontalMovement = UKismetMathLibrary::Lerp(WalkAnimMinHorizontalMovement, WalkAnimMaxHorizontalMovement, Value);
}

void AShooterCharacter::Handle_OnWalkAnimTimelineVerticalMovementUpdate(float Value)
{
	ProceduralAnimVerticalMovement = UKismetMathLibrary::Lerp(WalkAnimMinVerticalMovement, WalkAnimMaxVerticalMovement, Value);
}

void AShooterCharacter::Handle_OnWalkAnimTimelineRollRotationUpdate(float Value)
{
	ProceduralAnimRollRotation = UKismetMathLibrary::Lerp(WalkAnimMinRollRotation, WalkAnimMaxRollRotation, Value);
}

void AShooterCharacter::Handle_OnTurningAnimTimelineUpdate(float Value)
{
	float DeltaReference = UKismetMathLibrary::NormalizedDeltaRotator(FRotator(0.0f, GetActorRotation().Yaw, 0.0f), FRotator(0.0f, ReferenceActorRotationYaw, 0.0f)).Yaw;
	ReferenceActorRotationYaw += DeltaReference * Value;
}

void AShooterCharacter::Handle_OnTurningAnimTimelineFinished()
{
	TurningDirection = ETurningDirection::TD_None;
}

//void AShooterCharacter::Handle_OnCharacterAnimInstanceIdle()
//{
//	if (WalkAnimTimeline && WalkAnimTimeline->IsPlaying())
//	{
//		WalkAnimTimeline->Stop();
//	}
//	if (IdleAnimTimeline && !IdleAnimTimeline->IsPlaying())
//	{
//		IdleAnimTimeline->PlayFromStart();
//	}
//}

//void AShooterCharacter::Handle_OnCharacterAnimInstanceWalk()
//{
//	if (IdleAnimTimeline && IdleAnimTimeline->IsPlaying())
//	{
//		IdleAnimTimeline->Stop();
//	}
//	if (WalkAnimTimeline && !WalkAnimTimeline->IsPlaying())
//	{
//		WalkAnimTimeline->PlayFromStart();
//	}
//}

//void AShooterCharacter::Handle_OnCharacterAnimInstanceThirdOrCombatAction()
//{
//	if (IdleAnimTimeline && IdleAnimTimeline->IsPlaying())
//	{
//		IdleAnimTimeline->Stop();
//	}
//	if (WalkAnimTimeline && WalkAnimTimeline->IsPlaying())
//	{
//		WalkAnimTimeline->Stop();
//	}
//}

//void AShooterCharacter::Handle_OnCharacterAnimInstanceTurningInPlace(ETurningDirection NewTurningDirection)
//{
//	TurningDirection = NewTurningDirection;
//}

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

void AShooterCharacter::Handle_OnCombatComponentEquippedWeaponChanged(AWeapon* Weapon)
{
	if (HandsMesh)
	{
		USkinnedMeshComponent* NewLeaderPoseComponent = Weapon != nullptr ? Weapon->GetMesh() : nullptr;
		HandsMesh->SetLeaderPoseComponent(NewLeaderPoseComponent);
	}
}

void AShooterCharacter::Handle_OnInventoryComponentWeaponArrayReplicated()
{
	if (CharacterInventory)
	{
		AWeapon* PrimaryWeapon = CharacterInventory->GetWeaponAtIndex(PRIMARY_WEAPON_INDEX);
		if (CharacterCombat)
		{
			CharacterCombat->EquipWeapon(PrimaryWeapon);
		}
		Server_EquipWeapon(PrimaryWeapon);
	}
}

void AShooterCharacter::Handle_OnMovementComponentSprint()
{
	if (LeaningDirection != ELeaningDirection::LD_None)
	{
		LeaningDirection = ELeaningDirection::LD_None;
		LeaningTargetAngle = 0.0f;
		LeaningTransitionDuration = LeaningDefaultTransitionDuration;
	}
}

void AShooterCharacter::OnRep_LeaningDirection(ELeaningDirection OldLeaningDirection)
{
	CalculateLeaningTargetAngle();
	CalculateLeaningTransitionDuration(OldLeaningDirection);
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
	bool CurrentValue = Value.Get<bool>();
	bIsAlterAction = CurrentValue;
}

void AShooterCharacter::OnCharacterCrouchAction(const FInputActionValue& Value)
{
	bool CurrentValue = Value.Get<bool>();
	if (CurrentValue && !bIsTransition)
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
	bool CurrentValue = Value.Get<bool>();
	if (CurrentValue && CharacterInventory)
	{
		AWeapon* PrimaryWeapon = CharacterInventory->GetWeaponAtIndex(PRIMARY_WEAPON_INDEX);
		if (CharacterCombat)
		{
			CharacterCombat->EquipWeapon(PrimaryWeapon);
		}
		Server_EquipWeapon(PrimaryWeapon);
	}
}

void AShooterCharacter::OnCharacterEquipSecondaryWeaponAction(const FInputActionValue& Value)
{
	bool CurrentValue = Value.Get<bool>();
	if (CurrentValue && CharacterInventory)
	{
		AWeapon* SecondaryWeapon = CharacterInventory->GetWeaponAtIndex(SECONDARY_WEAPON_INDEX);
		if (CharacterCombat)
		{
			CharacterCombat->EquipWeapon(SecondaryWeapon);
		}
		Server_EquipWeapon(SecondaryWeapon);
	}
}

void AShooterCharacter::OnCharacterHolsterWeaponAction(const FInputActionValue& Value)
{
	bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		if (CharacterCombat)
		{
			CharacterCombat->EquipWeapon(nullptr);
		}
		Server_EquipWeapon(nullptr);
	}
}

void AShooterCharacter::OnCharacterLeanLeftAction(const FInputActionValue& Value)
{
	bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		ELeaningDirection NewLeaningDirection = LeaningDirection == ELeaningDirection::LD_Left ? ELeaningDirection::LD_None : ELeaningDirection::LD_Left;
		Server_Lean(NewLeaningDirection);
	}
}

void AShooterCharacter::OnCharacterLeanRightAction(const FInputActionValue& Value)
{
	bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		ELeaningDirection NewLeaningDirection = LeaningDirection == ELeaningDirection::LD_Right ? ELeaningDirection::LD_None : ELeaningDirection::LD_Right;
		Server_Lean(NewLeaningDirection);
	}
}

void AShooterCharacter::OnCharacterLookAction(const FInputActionValue& Value)
{
	FVector2D CurrentValue = Value.Get<FVector2D>();

	AddControllerYawInput(CurrentValue.X);
	AddControllerPitchInput(CurrentValue.Y);

}

void AShooterCharacter::OnCharacterMoveBackwardAction(const FInputActionValue& Value)
{
	float CurrentValue = Value.Get<float>();
	if (!bIsTransition)
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
			if (bIsProned && LeaningDirection != ELeaningDirection::LD_None)
			{
				Server_Lean(ELeaningDirection::LD_None);
			}
		}
	}
}

void AShooterCharacter::OnCharacterMoveForwardAction(const FInputActionValue& Value)
{
	float CurrentValue = Value.Get<float>();
	if (!bIsTransition)
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
			if (bIsProned && LeaningDirection != ELeaningDirection::LD_None)
			{
				Server_Lean(ELeaningDirection::LD_None);
			}
		}
	}
}

void AShooterCharacter::OnCharacterMoveLeftAction(const FInputActionValue& Value)
{
	float CurrentValue = Value.Get<float>();
	if (!bIsTransition)
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
			if (bIsProned && LeaningDirection != ELeaningDirection::LD_None)
			{
				Server_Lean(ELeaningDirection::LD_None);
			}
		}
	}
}

void AShooterCharacter::OnCharacterMoveRightAction(const FInputActionValue& Value)
{
	float CurrentValue = Value.Get<float>();
	if (!bIsTransition)
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
			if (bIsProned && LeaningDirection != ELeaningDirection::LD_None)
			{
				Server_Lean(ELeaningDirection::LD_None);
			}
		}
	}
}

void AShooterCharacter::OnCharacterProneAction(const FInputActionValue& Value)
{
	bool CurrentValue = Value.Get<bool>();
	if (CurrentValue && !bIsTransition)
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
	bool CurrentValue = Value.Get<bool>();
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
	bool CurrentValue = Value.Get<bool>();
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
	bool CurrentValue = Value.Get<bool>();
	if (CurrentValue && bIsAlterAction)
	{
		if (CharacterCombat)
		{
			CharacterCombat->CheckWeaponChamber();
		}
		Server_CheckWeaponChamber();
	}
}

void AShooterCharacter::OnWeaponFireAction(const FInputActionValue& Value)
{
	bool CurrentValue = Value.Get<bool>();
	if (CharacterCombat)
	{
		CharacterCombat->FireWeapon(CurrentValue);
	}
	Server_FireWeapon(CurrentValue);
}

void AShooterCharacter::OnWeaponFiremodeAction(const FInputActionValue& Value)
{
	bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		if (bIsAlterAction)
		{
			if (CharacterCombat)
			{
				CharacterCombat->CheckWeaponFiremode();
			}
			Server_CheckWeaponFiremode();
		}
		else
		{
			if (CharacterCombat)
			{
				CharacterCombat->ChangeWeaponFiremode();
			}
			Server_ChangeWeaponFiremode();
		}
	}
}

void AShooterCharacter::OnWeaponReloadAction(const FInputActionValue& Value)
{
	bool CurrentValue = Value.Get<bool>();
	if (CurrentValue)
	{
		if (bIsAlterAction)
		{
			if (CharacterCombat)
			{
				CharacterCombat->CheckWeaponMag();
			}
			Server_CheckWeaponMag();
		}
		else
		{
			if (CharacterCombat)
			{
				CharacterCombat->ReloadWeapon();
			}
			Server_ReloadWeapon();
		}
	}
}

void AShooterCharacter::Server_EquipWeapon_Implementation(AWeapon* WeaponToEquip)
{
	if (CharacterCombat)
	{
		CharacterCombat->EquipWeapon(WeaponToEquip);
	}
}

void AShooterCharacter::Server_ReloadWeapon_Implementation()
{
	if (CharacterCombat)
	{
		CharacterCombat->ReloadWeapon();
	}
}

void AShooterCharacter::Server_CheckWeaponChamber_Implementation()
{
	if (CharacterCombat)
	{
		CharacterCombat->CheckWeaponChamber();
	}
}

void AShooterCharacter::Server_FireWeapon_Implementation(bool bFire)
{
	if (CharacterCombat)
	{
		CharacterCombat->FireWeapon(bFire);
	}
}

void AShooterCharacter::Server_ChangeWeaponFiremode_Implementation()
{
	if (CharacterCombat)
	{
		CharacterCombat->ChangeWeaponFiremode();
	}
}

void AShooterCharacter::Server_CheckWeaponFiremode_Implementation()
{
	if (CharacterCombat)
	{
		CharacterCombat->CheckWeaponFiremode();
	}
}

void AShooterCharacter::Server_CheckWeaponMag_Implementation()
{
	if (CharacterCombat)
	{
		CharacterCombat->CheckWeaponMag();
	}
}

void AShooterCharacter::CalculateADSCameraTargetLocation()
{
	FTransform AimCameraTransform;
	if (AWeapon* EquippedWeapon = CharacterCombat ? CharacterCombat->GetEquippedWeapon() : nullptr)
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

		FTransform HandsAimCameraTransform = HandsMesh->GetSocketTransform(AIM_CAMERA_SOCKET_NAME, ERelativeTransformSpace::RTS_World);
		HandsAimCameraTransform = FShooterUtility::TransformToBoneSpace(HandsMesh, CAMERA_ANIMATED_SOCKET_NAME, HandsAimCameraTransform);
		if (ADSCameraTargetLocation.Y > HandsAimCameraTransform.GetLocation().Y)
		{
			ADSCameraTargetLocation.Y = HandsAimCameraTransform.GetLocation().Y;
		}
	}
}

void AShooterCharacter::CalculateLeaningTargetAngle()
{
	switch (LeaningDirection)
	{
	case ELeaningDirection::LD_None:
		LeaningTargetAngle = 0.0f;
		break;
	case ELeaningDirection::LD_Left:
		LeaningTargetAngle = -LeaningMaxAngle;
		break;
	case ELeaningDirection::LD_Right:
		LeaningTargetAngle = LeaningMaxAngle;
		break;
	case ELeaningDirection::Default_MAX:
		break;
	default:
		break;
	}
}

void AShooterCharacter::CalculateLeaningTransitionDuration(ELeaningDirection OldLeaningDirection)
{
	if ((LeaningDirection == ELeaningDirection::LD_Left && OldLeaningDirection == ELeaningDirection::LD_Right) ||
		(LeaningDirection == ELeaningDirection::LD_Right && OldLeaningDirection == ELeaningDirection::LD_Left))
	{
		LeaningTransitionDuration = LeaningDefaultTransitionDuration * 2.0f;
	}
	else
	{
		LeaningTransitionDuration = LeaningDefaultTransitionDuration;
	}
}
