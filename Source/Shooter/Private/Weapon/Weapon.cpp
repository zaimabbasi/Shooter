// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Ammo/Ammo.h"
#include "Animation/WeaponAnimInstance.h"
#include "Character/ShooterCharacter.h"
#include "Components/WeaponModComponent.h"
#include "Data/MagDataAsset.h"
#include "Data/WeaponModDataAsset.h"
#include "Data/WeaponDataAsset.h"
#include "Mod/Foregrip.h"
#include "Mod/Handguard.h"
#include "Mod/Mag.h"
#include "Mod/Mod.h"
#include "Mod/Scope.h"
#include "Mod/SightRear.h"
#include "Types/CombatTypes.h"
#include "Types/ShooterNames.h"
#include "Types/WeaponTypes.h"
#include <random>

AWeapon::AWeapon() :
	FiremodeIndex(0)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
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
			WeaponAnimInstance->OnWeaponAnimInstanceIdleStart.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceIdleStart);
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

void AWeapon::PlayAudio() const
{
	if (AudioComponent)
	{
		AudioComponent->Play();
	}
}

void AWeapon::StopAudio() const
{
	if (AudioComponent)
	{
		AudioComponent->Stop();
	}
}

void AWeapon::TriggerFireSound() const
{
	if (AudioComponent)
	{
		AudioComponent->SetTriggerParameter(FIRE_TRIGGER_NAME);
	}
}

void AWeapon::StopFireSound() const
{
	if (AudioComponent)
	{
		AudioComponent->SetTriggerParameter(FIRE_STOP_TRIGGER_NAME);
	}
}

void AWeapon::TriggerFireDrySound() const
{
	if (AudioComponent)
	{
		AudioComponent->SetTriggerParameter(bIsArmed ? HAMMER_TRIGGER_NAME : TRIGGER_TRIGGER_NAME);
	}
}

void AWeapon::TriggerCatchSound() const
{
	if (AudioComponent)
	{
		AudioComponent->SetTriggerParameter(CATCH_TRIGGER_NAME);
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
			SpawnedAmmo->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, SHELL_PORT_SOCKET_NAME);
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

void AWeapon::Multicast_ProxySpawnShellPortAmmo_Implementation(TSubclassOf<AAmmo> AmmoClass)
{
	if (!HasNetOwner())
	{
		SpawnShellPortAmmo(AmmoClass);
	}
}

void AWeapon::Multicast_ProxyEjectShellPortAmmo_Implementation()
{
	if (!HasNetOwner())
	{
		EjectShellPortAmmo();
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

			TriggerCatchSound();
		}
	}
}

void AWeapon::Handle_OnWeaponAnimInstanceIdleStart()
{
	StopFireSound();
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
	if (HasNetOwner())
	{
		EjectShellPortAmmo();
	}
	if (HasAuthority())
	{
		Multicast_ProxyEjectShellPortAmmo();
	}
}

void AWeapon::Handle_OnWeaponAnimInstanceSoundDryFire()
{
	TriggerFireDrySound();
}

void AWeapon::Handle_OnWeaponAnimInstanceWeaponHammer()
{
	if (AMag* Mag = GetMag())
	{
		if (PatronInWeaponAmmo)
		{
			PatronInWeaponAmmo->AttachToComponent(Mag->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, PATRON_SOCKET_NAME(Mag->GetAmmoCount() + 1));
			PatronInWeaponAmmo->SetActorHiddenInGame(true);
			PatronInWeaponAmmo = nullptr;
		}

		if (Mag->GetMagDataAsset().LoadSynchronous())
		{
			if (HasNetOwner())
			{
				SpawnShellPortAmmo(Mag->GetMagDataAsset()->AmmoClass);
			}
			if (HasAuthority())
			{
				Multicast_ProxySpawnShellPortAmmo(Mag->GetMagDataAsset()->AmmoClass);
			}
		}
	}

	if (HasNetOwner())
	{
		LineTrace();
		GenerateRecoil();
	}

	TriggerFireSound();

}

void AWeapon::Handle_OnWeaponAnimInstanceWeaponSelector()
{
	if (HasAuthority())
	{
		FiremodeIndex = FMath::Modulo<uint8>(FiremodeIndex + 1, GetNumFiremodes());
	}
}

void AWeapon::LineTrace()
{
	UWorld* const World = GetWorld();
	FTransform FireportTransform = Mesh != nullptr ? Mesh->GetSocketTransform(TEXT("fireport"), ERelativeTransformSpace::RTS_World) : FTransform();
	FVector Start = FireportTransform.GetLocation();
	FVector End = Start + (UKismetMathLibrary::GetRightVector(FireportTransform.Rotator()) * 2000.0f);
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	FHitResult OutHit;

	bool bHit = false;

	bHit = World->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility);

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);

	if (bHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Single Trace Object Name: %s"), *OutHit.HitObjectHandle.GetName());

		//FString ActorDisplayName = UKismetSystemLibrary::GetDisplayName(OutHit.GetActor());
		//UE_LOG(LogTemp, Warning, TEXT("Profile Trace OutHit Display Name: %s"), *ActorDisplayName);
	}
}
