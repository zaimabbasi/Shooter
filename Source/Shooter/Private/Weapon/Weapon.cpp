// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "Ammo/Ammo.h"
#include "Animation/WeaponAnimInstance.h"
#include "Character/ShooterCharacter.h"
#include "Components/WeaponModComponent.h"
#include "Data/MagDataAsset.h"
#include "Data/ModDataAsset.h"
#include "Data/WeaponDataAsset.h"
#include "Mod/Foregrip.h"
#include "Mod/Handguard.h"
#include "Mod/Mag.h"
#include "Mod/Mod.h"
#include "Mod/Scope.h"
#include "Mod/SightRear.h"
#include "Types/ShooterNames.h"
#include "Types/WeaponTypes.h"

AWeapon::AWeapon() :
	bIsHolster(false),
	FiremodeIndex(0)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(GetRootComponent());

	WeaponModComponent = CreateDefaultSubobject<UWeaponModComponent>(TEXT("ModComponent"));
	WeaponModComponent->SetIsReplicated(true);
	
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
			WeaponAnimInstance->OnWeaponAnimInstanceIdle.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceIdle);
			WeaponAnimInstance->OnWeaponAnimInstanceIdleToOut.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceIdleToOut);
			WeaponAnimInstance->OnWeaponAnimInstanceMagCheck.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceMagCheck);
			WeaponAnimInstance->OnWeaponAnimInstanceMagIn.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceMagIn);
			WeaponAnimInstance->OnWeaponAnimInstanceMagOut.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceMagOut);
			WeaponAnimInstance->OnWeaponAnimInstanceOut.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceOut);
			WeaponAnimInstance->OnWeaponAnimInstanceOutToIdle.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceOutToIdle);
			WeaponAnimInstance->OnWeaponAnimInstanceOutToIdleArm.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceOutToIdleArm);
			WeaponAnimInstance->OnWeaponAnimInstanceReloadCharge.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceReloadCharge);
			WeaponAnimInstance->OnWeaponAnimInstancePatronInWeapon.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstancePatronInWeapon);
			WeaponAnimInstance->OnWeaponAnimInstanceWeaponSelector.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceWeaponSelector);
			WeaponAnimInstance->OnWeaponAnimInstanceWeaponHammer.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceWeaponHammer);
			WeaponAnimInstance->OnWeaponAnimInstanceShellPort.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceShellPort);
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

	DOREPLIFETIME(AWeapon, bIsHolster);
	DOREPLIFETIME(AWeapon, FiremodeIndex);
	DOREPLIFETIME(AWeapon, PatronInWeaponAmmo);
	DOREPLIFETIME(AWeapon, ShellPortAmmo);

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
			WeaponModComponent->Init(LoadedWeaponDataAsset->ModDataAsset.LoadSynchronous());
		}
	}

	if (AMag* Mag = GetMag())
	{
		Mag->OnMagAmmoPopped.AddDynamic(this, &AWeapon::Handle_OnMagAmmoPopped);
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

void AWeapon::Handle_OnMagAmmoPopped(AAmmo* PoppedAmmo)
{
	AMag* Mag = GetMag();
	if (PoppedAmmo == nullptr && Mag)
	{
		if (const UMagDataAsset* MagDataAsset = Mag->GetMagDataAsset().LoadSynchronous())
		{
			if (UWorld* World = GetWorld())
			{
				PoppedAmmo = World->SpawnActor<AAmmo>(MagDataAsset->AmmoClass);
			}
		}
	}
	if (PoppedAmmo)
	{
		PoppedAmmo->SetOwner(this);
		PoppedAmmo->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, PATRON_IN_WEAPON_SOCKET_NAME);
	}
	PatronInWeaponAmmo = PoppedAmmo;
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

void AWeapon::Handle_OnWeaponAnimInstancePatronInWeapon()
{
	if (HasAuthority())
	{
		if (AMag* Mag = GetMag())
		{
			Mag->PopAmmo();
		}
	}
}

void AWeapon::Handle_OnWeaponAnimInstanceReloadCharge()
{
	OnWeaponReloadCharge.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceShellPort()
{
	if (ShellPortAmmo)
	{
		ShellPortAmmo->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		if (USkeletalMeshComponent* ShellPortAmmoMesh = ShellPortAmmo->GetMesh())
		{
			ShellPortAmmoMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
			ShellPortAmmoMesh->SetSimulatePhysics(true);
			ShellPortAmmoMesh->SetEnableGravity(true);
			FVector ImpulseVector = (-3.0f * ShellPortAmmoMesh->GetForwardVector()) + (-2.0f * ShellPortAmmoMesh->GetRightVector()) + (3.0f * ShellPortAmmoMesh->GetUpVector());
			ShellPortAmmoMesh->AddImpulse(ImpulseVector);
		}
	}
}

void AWeapon::Handle_OnWeaponAnimInstanceWeaponHammer()
{
	if (HasAuthority())
	{
		if (PatronInWeaponAmmo)
		{
			PatronInWeaponAmmo->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, SHELL_PORT_SOCKET_NAME);
			PatronInWeaponAmmo->SetIsEmpty(true);
			ShellPortAmmo = PatronInWeaponAmmo;
			PatronInWeaponAmmo = nullptr;
		}
	}
}

void AWeapon::Handle_OnWeaponAnimInstanceWeaponSelector()
{
	if (HasAuthority())
	{
		FiremodeIndex = FMath::Modulo<uint8>(FiremodeIndex + 1, GetNumFiremodes());
	}
}
