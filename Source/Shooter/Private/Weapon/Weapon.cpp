// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Ammo/Ammo.h"
#include "Animation/WeaponAnimInstance.h"
#include "Character/ShooterCharacter.h"
#include "Components/WeaponModComponent.h"
#include "Data/MagDataAsset.h"
#include "Data/WeaponModDataAsset.h"
#include "Data/WeaponDataAsset.h"
#include "Interface/Impactable.h"
#include "Mod/Foregrip.h"
#include "Mod/Handguard.h"
#include "Mod/Mag.h"
#include "Mod/Mod.h"
#include "Mod/Muzzle.h"
#include "Mod/Scope.h"
#include "Mod/SightRear.h"
#include "Types/CombatTypes.h"
#include "Types/ShooterNames.h"
#include "Types/WeaponTypes.h"
#include "Utility/ShooterUtility.h"
#include <random>

AWeapon::AWeapon() :
	FiremodeIndex(0)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(GetRootComponent());
	AudioComponent->SetComponentTickEnabled(false);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(GetRootComponent());

	WeaponModComponent = CreateDefaultSubobject<UWeaponModComponent>(TEXT("ModComponent"));
	WeaponModComponent->SetIsReplicated(true);

	RecoilKickMaxDistance = 2.0f;
	RecoilKickMaxPitchRotation = 0.5f;
	
}

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Mesh)
	{
		if (UWeaponAnimInstance* WeaponAnimInstance = Cast<UWeaponAnimInstance>(Mesh->GetAnimInstance()))
		{
			WeaponAnimInstance->OnWeaponAnimInstanceActionEnd.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceActionEnd);
			WeaponAnimInstance->OnWeaponAnimInstanceActionStart.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceActionStart);
			WeaponAnimInstance->OnWeaponAnimInstanceChamberCheck.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceChamberCheck);
			WeaponAnimInstance->OnWeaponAnimInstanceFire.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceFire);
			WeaponAnimInstance->OnWeaponAnimInstanceFireDry.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceFireDry);
			WeaponAnimInstance->OnWeaponAnimInstanceFiremode.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceFiremode);
			WeaponAnimInstance->OnWeaponAnimInstanceFiremodeCheck.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceFiremodeCheck);
			WeaponAnimInstance->OnWeaponAnimInstanceHandsInHalf1.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceHandsInHalf1);
			WeaponAnimInstance->OnWeaponAnimInstanceHandsInHalf2.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceHandsInHalf2);
			WeaponAnimInstance->OnWeaponAnimInstanceHandsOutHalf1.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceHandsOutHalf1);
			WeaponAnimInstance->OnWeaponAnimInstanceHandsOutHalf2.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceHandsOutHalf2);
			WeaponAnimInstance->OnWeaponAnimInstanceIdle.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceIdle);
			WeaponAnimInstance->OnWeaponAnimInstanceIdleToOut.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceIdleToOut);
			WeaponAnimInstance->OnWeaponAnimInstanceMagCheck.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceMagCheck);
			WeaponAnimInstance->OnWeaponAnimInstanceMagIn.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceMagIn);
			WeaponAnimInstance->OnWeaponAnimInstanceMagOut.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceMagOut);
			WeaponAnimInstance->OnWeaponAnimInstanceOut.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceOut);
			WeaponAnimInstance->OnWeaponAnimInstanceOutToIdle.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceOutToIdle);
			WeaponAnimInstance->OnWeaponAnimInstanceOutToIdleArm.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceOutToIdleArm);
			WeaponAnimInstance->OnWeaponAnimInstanceReloadCatch.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceReloadCatch);
			WeaponAnimInstance->OnWeaponAnimInstanceReloadCharge.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceReloadCharge);
			WeaponAnimInstance->OnWeaponAnimInstanceBoltCatch.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceBoltCatch);
			WeaponAnimInstance->OnWeaponAnimInstancePatronInWeapon.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstancePatronInWeapon);
			WeaponAnimInstance->OnWeaponAnimInstanceWeaponSelector.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceWeaponSelector);
			WeaponAnimInstance->OnWeaponAnimInstanceWeaponHammer.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceWeaponHammer);
			WeaponAnimInstance->OnWeaponAnimInstanceShellPort.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceShellPort);
			WeaponAnimInstance->OnWeaponAnimInstanceSoundDryFire.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceSoundDryFire);
		}
	}

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

}

//void AWeapon::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, FiremodeIndex);

}

void AWeapon::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);

	ShooterCharacterOwner = Cast<AShooterCharacter>(NewOwner);
}

void AWeapon::Init()
{
	if (WeaponModComponent)
	{
		if (const UWeaponDataAsset* LoadedWeaponDataAsset = WeaponDataAsset.LoadSynchronous())
		{
			WeaponModComponent->Init(LoadedWeaponDataAsset->WeaponModDataAsset.LoadSynchronous());
		}
	}

}

bool AWeapon::GetIsOneHanded() const
{
	const UWeaponDataAsset* LoadedWeaponDataAsset = WeaponDataAsset.LoadSynchronous();
	return LoadedWeaponDataAsset && LoadedWeaponDataAsset->IsOneHanded;
}

uint16 AWeapon::GetRateOfFire() const
{
	const UWeaponDataAsset* LoadedWeaponDataAsset = WeaponDataAsset.LoadSynchronous();
	return LoadedWeaponDataAsset ? LoadedWeaponDataAsset->RateOfFire : 0;
}

uint8 AWeapon::GetNumFiremodes() const
{
	const UWeaponDataAsset* LoadedWeaponDataAsset = WeaponDataAsset.LoadSynchronous();
	return LoadedWeaponDataAsset ? LoadedWeaponDataAsset->Firemodes.Num() : 0;
}

EWeaponFiremode AWeapon::GetFiremode() const
{
	const UWeaponDataAsset* LoadedWeaponDataAsset = WeaponDataAsset.LoadSynchronous();
	return LoadedWeaponDataAsset && LoadedWeaponDataAsset->Firemodes.IsValidIndex(FiremodeIndex) ? LoadedWeaponDataAsset->Firemodes[FiremodeIndex] : EWeaponFiremode::WF_None;
}

//AForegrip* AWeapon::GetForegrip() const
//{
//	return GetAttachedActor<AForegrip>();
//}

//AHandguard* AWeapon::GetHandguard() const
//{
//	return GetAttachedActor<AHandguard>();
//}

AMag* AWeapon::GetMag() const
{
	if (WeaponModComponent == nullptr)
	{
		return nullptr;
	}
	return WeaponModComponent->GetMod<AMag>();
}

AMuzzle* AWeapon::GetMuzzle() const
{
	return WeaponModComponent != nullptr ? WeaponModComponent->GetMod<AMuzzle>() : nullptr;
}

USkeletalMeshComponent* AWeapon::GetForegripHandguardMesh() const
{
	if (WeaponModComponent == nullptr)
	{
		return nullptr;
	}
	AMod* ModForegrip = WeaponModComponent->GetMod<AForegrip>();
	AMod* ModHandguard = WeaponModComponent->GetMod<AHandguard>();
	return ModForegrip != nullptr ? ModForegrip->GetMesh() : ModHandguard != nullptr ? ModHandguard->GetMesh() : nullptr;
}

USkeletalMeshComponent* AWeapon::GetScopeSightMesh() const
{
	if (WeaponModComponent == nullptr)
	{
		return nullptr;
	}
	AMod* ModScope = WeaponModComponent->GetMod<AScope>();
	AMod* ModSightRear = WeaponModComponent->GetMod<ASightRear>();
	return ModScope != nullptr ? ModScope->GetMesh() : ModSightRear != nullptr ? ModSightRear->GetMesh() : nullptr;
}

UAnimInstance* AWeapon::GetAnimInstance() const
{
	return Mesh != nullptr ? Mesh->GetAnimInstance() : nullptr;
}

ECombatAction AWeapon::GetWeaponAnimCombatAction() const
{
	if (UWeaponAnimInstance* WeaponAnimInstance = Cast<UWeaponAnimInstance>(GetAnimInstance()))
	{
		return WeaponAnimInstance->GetCombatAction();
	}
	return ECombatAction::CA_None;
}

void AWeapon::SetWeaponAnimCombatAction(ECombatAction CombatAction) const
{
	if (UWeaponAnimInstance* WeaponAnimInstance = Cast<UWeaponAnimInstance>(GetAnimInstance()))
	{
		WeaponAnimInstance->SetCombatAction(CombatAction);
	}
}

//void AWeapon::SpawnMuzzleSmokeEffect()
//{
//	if (UKismetMathLibrary::RandomBoolWithWeight(1.0f))
//	{
//		if (AMuzzle* Muzzle = GetMuzzle())
//		{
//			MuzzleSmokeComponent = Muzzle->SpawnMuzzleSmokeEffect(MuzzleSmokeSystem.LoadSynchronous());
//		}
//		else
//		{
//			MuzzleSmokeComponent = FShooterUtility::SpawnNiagaraSystemAttached(MuzzleSmokeSystem.LoadSynchronous(), Mesh, FIREPORT_SOCKET_NAME, true);
//		}
//	}
//}

//void AWeapon::DeactivateMuzzleSmokeEffect() const
//{
//	if (MuzzleSmokeComponent && MuzzleSmokeComponent->IsActive())
//	{
//		MuzzleSmokeComponent->Deactivate();
//	}
//}

void AWeapon::ResetNumRoundsFired()
{
	NumRoundsFired = 0;
}

void AWeapon::OnFireEnd()
{
	if (AudioComponent)
	{
		AudioComponent->SetTriggerParameter(FIRE_STOP_TRIGGER_NAME);
	}

	if (HasAuthority())
	{
		Multicast_ProxyOnFireEnd();
	}
}

void AWeapon::SpawnShellPortAmmo(TSubclassOf<AAmmo> AmmoClass)
{
	if (UWorld* World = GetWorld())
	{
		if (AAmmo* SpawnedAmmo = World->SpawnActor<AAmmo>(AmmoClass))
		{
			SpawnedAmmo->SetOwner(this);
			SpawnedAmmo->SetReplicates(false);
			SpawnedAmmo->SetIsEmpty(true);
			SpawnedAmmo->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, SHELLPORT_SOCKET_NAME);
			SpawnedAmmo->SetLifeSpan(2.0f);

			ShellPortAmmo = SpawnedAmmo;
		}
	}
}

void AWeapon::EjectShellPortAmmo()
{
	if (ShellPortAmmo)
	{
		ShellPortAmmo->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		if (ShellPortAmmo->GetMesh())
		{
			ShellPortAmmo->GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
			ShellPortAmmo->GetMesh()->SetSimulatePhysics(true);
			ShellPortAmmo->GetMesh()->SetEnableGravity(true);

			FVector ImpulseVector = (-3.0f * ShellPortAmmo->GetMesh()->GetForwardVector()) + (-2.0f * ShellPortAmmo->GetMesh()->GetRightVector()) + (3.0f * ShellPortAmmo->GetMesh()->GetUpVector());
			ShellPortAmmo->GetMesh()->AddImpulse(ImpulseVector);
		}
		ShellPortAmmo = nullptr;
	}
}

void AWeapon::GenerateRecoil()
{
	float VerticalKick = WeaponDataAsset.LoadSynchronous() ? WeaponDataAsset->VerticalKick : 0.0f;
	float HorizontalSpread = WeaponDataAsset.LoadSynchronous() ? WeaponDataAsset->HorizontalSpread : 0.0f;
	float HorizontalSpreadDirection = WeaponDataAsset.LoadSynchronous() ? WeaponDataAsset->HorizontalSpreadDirection : 0.0f;

	std::random_device rd;
	std::mt19937 mt19937(rd());
	std::normal_distribution<float> nd = std::normal_distribution<float>(HorizontalSpread * HorizontalSpreadDirection, HorizontalSpread);

	float HorizontalKick = nd(mt19937);

	OnWeaponRecoilGenerated.Broadcast(this, HorizontalKick, VerticalKick);
}

bool AWeapon::PerformLineTrace(FHitResult& OutHit)
{
	if (UWorld* const World = GetWorld())
	{
		FTransform FireportTransform = Mesh != nullptr ? Mesh->GetSocketTransform(FIREPORT_SOCKET_NAME, ERelativeTransformSpace::RTS_World) : FTransform();
		FVector Start = FireportTransform.GetLocation();
		FVector End = Start + (UKismetMathLibrary::GetRightVector(FireportTransform.Rotator()) * 2000.0f);

		FCollisionQueryParams CollisionQueryParams = FCollisionQueryParams::DefaultQueryParam;
		CollisionQueryParams.AddIgnoredActor(this);
		CollisionQueryParams.bReturnPhysicalMaterial = true;

		//DrawDebugLine(World, Start, End, FColor::Green, false, 1, 0, 1);

		return World->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionQueryParams);
	}

	return false;
}

void AWeapon::Multicast_ProxyOnFireEnd_Implementation()
{
	if (!HasNetOwner() && AudioComponent)
	{
		AudioComponent->SetTriggerParameter(FIRE_STOP_TRIGGER_NAME);
	}
}

void AWeapon::Multicast_ProxyHandle_OnWeaponAnimInstanceBoltCatch_Implementation(bool bTriggerCatchSound)
{
	if (!HasNetOwner() && AudioComponent && bTriggerCatchSound)
	{
		AudioComponent->SetTriggerParameter(CATCH_TRIGGER_NAME);
	}
}

void AWeapon::Multicast_ProxyHandle_OnWeaponAnimInstanceShellPort_Implementation()
{
	if (!HasNetOwner())
	{
		EjectShellPortAmmo();
	}
}

void AWeapon::Multicast_ProxyHandle_OnWeaponAnimInstanceSoundDryFire_Implementation(bool bArmed)
{
	if (!HasNetOwner() && AudioComponent)
	{
		AudioComponent->SetTriggerParameter(bArmed ? HAMMER_TRIGGER_NAME : TRIGGER_TRIGGER_NAME);
	}
}

void AWeapon::Multicast_ProxyHandle_OnWeaponAnimInstanceWeaponHammer_Implementation(const AMuzzle* Muzzle, TSubclassOf<AAmmo> AmmoClass, const FHitResult& HitResult, bool bBlockingHit, bool bTriggerFireSound)
{
	if (!HasNetOwner())
	{
		SpawnShellPortAmmo(AmmoClass);

		if (bBlockingHit)
		{
			if (UKismetSystemLibrary::DoesImplementInterface(HitResult.PhysMaterial.Get(), UImpactable::StaticClass()))
			{
				IImpactable::Execute_HandleBulletImpact(HitResult.PhysMaterial.Get(), HitResult);
			}
		}

		if (Muzzle)
		{
			Muzzle->SpawnMuzzleFlashEffect(MuzzleFlashSystem.LoadSynchronous());
		}
		else
		{
			FShooterUtility::SpawnNiagaraSystemAttached(MuzzleFlashSystem.LoadSynchronous(), Mesh, FIREPORT_SOCKET_NAME, true);
		}
		FShooterUtility::SpawnNiagaraSystemAttached(FireSmokeSystem.LoadSynchronous(), Mesh, SMOKEPORT_SOCKET_NAME, true);

		if (bTriggerFireSound && AudioComponent)
		{
			AudioComponent->SetTriggerParameter(FIRE_TRIGGER_NAME);
		}
	}
}

void AWeapon::Handle_OnWeaponAnimInstanceActionEnd()
{
	OnWeaponActionEnd.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceActionStart()
{
	OnWeaponActionStart.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceChamberCheck()
{
	OnWeaponChamberCheck.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceFire()
{
	++NumRoundsFired;

	OnWeaponFire.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceFireDry()
{
	bIsArmed = false;

	OnWeaponFireDry.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceFiremode()
{
	OnWeaponFiremode.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceFiremodeCheck()
{
	OnWeaponFiremodeCheck.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceHandsInHalf1()
{
	OnWeaponHandsInHalf1.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceHandsInHalf2()
{
	OnWeaponHandsInHalf2.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceHandsOutHalf1()
{
	OnWeaponHandsOutHalf1.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceHandsOutHalf2()
{
	OnWeaponHandsOutHalf2.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceIdle()
{
	OnWeaponIdle.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceIdleToOut()
{
	OnWeaponIdleToOut.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceMagCheck()
{
	OnWeaponMagCheck.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceMagIn()
{
	OnWeaponMagIn.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceMagOut()
{
	OnWeaponMagOut.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceOut()
{
	OnWeaponOut.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceOutToIdle()
{
	OnWeaponOutToIdle.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceOutToIdleArm()
{
	OnWeaponOutToIdleArm.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceReloadCatch()
{
	bIsBoltCatched = false;

	OnWeaponReloadCatch.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceReloadCharge()
{
	OnWeaponReloadCharge.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceBoltCatch()
{
	if (AMag* Mag = GetMag())
	{
		if (Mag->GetAmmoCount() == 0)
		{
			bIsBoltCatched = true;
			
			if (AudioComponent)
			{
				AudioComponent->SetTriggerParameter(CATCH_TRIGGER_NAME);
			}
		}
	}

	if (HasAuthority() && bIsBoltCatched)
	{
		Multicast_ProxyHandle_OnWeaponAnimInstanceBoltCatch(true);
	}
}

void AWeapon::Handle_OnWeaponAnimInstancePatronInWeapon()
{
	if (AMag* Mag = GetMag())
	{
		if (AAmmo* MagAmmo = Mag->PopAmmo())
		{
			MagAmmo->SetActorHiddenInGame(false);
			MagAmmo->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, PATRON_IN_WEAPON_SOCKET_NAME);

			PatronInWeaponAmmo = MagAmmo;

			bIsArmed = true;
		}
	}
}

void AWeapon::Handle_OnWeaponAnimInstanceShellPort()
{
	EjectShellPortAmmo();

	if (HasAuthority())
	{
		Multicast_ProxyHandle_OnWeaponAnimInstanceShellPort();
	}
}

void AWeapon::Handle_OnWeaponAnimInstanceSoundDryFire()
{
	if (AudioComponent)
	{
		AudioComponent->SetTriggerParameter(bIsArmed ? HAMMER_TRIGGER_NAME : TRIGGER_TRIGGER_NAME);
	}

	if (HasAuthority())
	{
		Multicast_ProxyHandle_OnWeaponAnimInstanceSoundDryFire(bIsArmed);
	}
}

void AWeapon::Handle_OnWeaponAnimInstanceWeaponHammer()
{
	AMag* Mag = GetMag();
	TSubclassOf<AAmmo> AmmoClass;
	if (Mag)
	{
		if (PatronInWeaponAmmo)
		{
			PatronInWeaponAmmo->AttachToComponent(Mag->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, PATRON_SOCKET_NAME(Mag->GetAmmoCount() + 1));
			PatronInWeaponAmmo->SetActorHiddenInGame(true);
			PatronInWeaponAmmo = nullptr;
		}
	
		if (Mag->GetMagDataAsset().LoadSynchronous())
		{
			AmmoClass = Mag->GetMagDataAsset()->AmmoClass;
			SpawnShellPortAmmo(AmmoClass);
		}
	}

	FHitResult OutHit;
	bool bBlockingHit = PerformLineTrace(OutHit);
	if (bBlockingHit)
	{
		/*UE_LOG(LogTemp, Warning, TEXT("Single Trace Object Name: %s"), *OutHit.HitObjectHandle.GetName());

		FString ActorDisplayName = UKismetSystemLibrary::GetDisplayName(OutHit.GetActor());
		UE_LOG(LogTemp, Warning, TEXT("Profile Trace OutHit Display Name: %s"), *ActorDisplayName);*/

		if (UKismetSystemLibrary::DoesImplementInterface(OutHit.PhysMaterial.Get(), UImpactable::StaticClass()))
		{
			IImpactable::Execute_HandleBulletImpact(OutHit.PhysMaterial.Get(), OutHit);
		}
	}
	
	GenerateRecoil();

	AMuzzle* Muzzle = GetMuzzle();
	if (Muzzle)
	{
		Muzzle->SpawnMuzzleFlashEffect(MuzzleFlashSystem.LoadSynchronous());
	}
	else
	{
		FShooterUtility::SpawnNiagaraSystemAttached(MuzzleFlashSystem.LoadSynchronous(), Mesh, FIREPORT_SOCKET_NAME, true);
	}
	FShooterUtility::SpawnNiagaraSystemAttached(FireSmokeSystem.LoadSynchronous(), Mesh, SMOKEPORT_SOCKET_NAME, true);

	if (NumRoundsFired == 0 && AudioComponent)
	{
		AudioComponent->SetTriggerParameter(FIRE_TRIGGER_NAME);
	}

	if (HasAuthority())
	{
		Multicast_ProxyHandle_OnWeaponAnimInstanceWeaponHammer(Muzzle, AmmoClass, OutHit, bBlockingHit, NumRoundsFired == 0);
	}

}

void AWeapon::Handle_OnWeaponAnimInstanceWeaponSelector()
{
	if (HasAuthority())
	{
		FiremodeIndex = FMath::Modulo<uint8>(FiremodeIndex + 1, GetNumFiremodes());
	}
}
