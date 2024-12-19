// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Weapon.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Actor/Ammo.h"
#include "Actor/Mag.h"
#include "ActorComponent/ModComponent.h"
#include "AnimInstance/WeaponAnimInstance.h"
#include "Character/ShooterCharacter.h"
#include "DataAsset/MagDataAsset.h"
#include "DataAsset/ModDataAsset.h"
#include "DataAsset/WeaponDataAsset.h"
#include "Type/ShooterNameType.h"

AWeapon::AWeapon() :
	bIsHolster(false),
	CombatAction(ECombatAction::CA_Out),
	FiremodeIndex(0)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(GetRootComponent());

	ModComponent = CreateDefaultSubobject<UModComponent>(TEXT("ModComponent"));
	ModComponent->SetIsReplicated(true);

}

bool AWeapon::DoesNeedCharge()
{
	return (GetMagAmmoCount() > 0 && PatronInWeaponAmmo == nullptr);
}

EWeaponFiremode AWeapon::GetFiremode() const
{
	const UWeaponDataAsset* LoadedWeaponDataAsset = WeaponDataAsset.LoadSynchronous();
	if (LoadedWeaponDataAsset == nullptr || !LoadedWeaponDataAsset->Firemodes.IsValidIndex(FiremodeIndex))
	{
		return EWeaponFiremode::WF_None;
	}
	return LoadedWeaponDataAsset->Firemodes[FiremodeIndex];
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, bIsHolster);
	DOREPLIFETIME(AWeapon, FiremodeIndex);
	DOREPLIFETIME(AWeapon, ShellPortAmmo);
	DOREPLIFETIME(AWeapon, PatronInWeaponAmmo);

}

bool AWeapon::GetIsOneHanded() const
{
	const UWeaponDataAsset* LoadedWeaponDataAsset = WeaponDataAsset.LoadSynchronous();
	if (LoadedWeaponDataAsset == nullptr)
	{
		return false;
	}
	return LoadedWeaponDataAsset->IsOneHanded;
}

uint8 AWeapon::GetMagAmmoCount() const
{
	if (ModComponent == nullptr || ModComponent->GetMag() == nullptr)
	{
		return 0;
	}
	return ModComponent->GetMag()->GetAmmoCount();
}

uint8 AWeapon::GetMagAmmoSpace() const
{
	if (ModComponent == nullptr)
	{
		return 0;
	}
	AMag* Mag = ModComponent->GetMag();
	if (Mag == nullptr)
	{
		return 0;
	}
	return Mag->GetAmmoSpace();
}

uint16 AWeapon::GetRateOfFire() const
{
	const UWeaponDataAsset* LoadedWeaponDataAsset = WeaponDataAsset.LoadSynchronous();
	if (LoadedWeaponDataAsset == nullptr)
	{
		return 0;
	}
	return LoadedWeaponDataAsset->RateOfFire;
}

bool AWeapon::HasFiremodes()
{
	const UWeaponDataAsset* LoadedWeaponDataAsset = WeaponDataAsset.LoadSynchronous();
	if (LoadedWeaponDataAsset && LoadedWeaponDataAsset->Firemodes.Num() > 1)
	{
		return true;
	}
	return false;
}

bool AWeapon::HasMag()
{
	return ModComponent && ModComponent->GetMag();
}

bool AWeapon::HasPatronInWeaponAmmo()
{
	return PatronInWeaponAmmo != nullptr;
}

void AWeapon::Init()
{
	if (ModComponent)
	{
		if (const UWeaponDataAsset* LoadedWeaponDataAsset = WeaponDataAsset.LoadSynchronous())
		{
			ModComponent->Init(LoadedWeaponDataAsset->ModDataAsset.LoadSynchronous());
		}
		if (AMag* Mag = ModComponent->GetMag())
		{
			Mag->OnMagAmmoPopped.AddDynamic(this, &AWeapon::Handle_OnMagAmmoPopped);
		}
	}
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

void AWeapon::Server_MagAddAmmo_Implementation(const uint8 Count)
{
	if (ModComponent == nullptr || ModComponent->GetMag() == nullptr)
	{
		return;
	}
	ModComponent->GetMag()->Server_AddAmmo(Count);
}

void AWeapon::Server_MagPopAmmo_Implementation()
{
	if (ModComponent == nullptr || ModComponent->GetMag() == nullptr)
	{
		return;
	}
	ModComponent->GetMag()->Server_PopAmmo();
}

void AWeapon::Server_SetIsHolster_Implementation(const bool bHolster)
{
	bIsHolster = bHolster;
}

void AWeapon::Server_SwitchFiremode_Implementation()
{
	if (!HasFiremodes())
	{
		return;
	}
	if (const UWeaponDataAsset* LoadedWeaponDataAsset = WeaponDataAsset.LoadSynchronous())
	{
		FiremodeIndex = FMath::Modulo<uint8>(FiremodeIndex + 1, LoadedWeaponDataAsset->Firemodes.Num());
	}
}

void AWeapon::SetCombatAction(ECombatAction Action)
{
	CombatAction = Action;
}

void AWeapon::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);

	ShooterCharacterOwner = Cast<AShooterCharacter>(NewOwner);
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

}

void AWeapon::EjectShellPortAmmo()
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

void AWeapon::Handle_OnMagAmmoPopped(AAmmo* PoppedAmmo)
{
	if (PoppedAmmo == nullptr && ModComponent && ModComponent->GetMag())
	{
		if (const UMagDataAsset* MagDataAsset = ModComponent->GetMag()->GetMagDataAsset().LoadSynchronous())
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
		Server_MagPopAmmo();
	}
}

void AWeapon::Handle_OnWeaponAnimInstanceReloadCharge()
{
	OnWeaponReloadCharge.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceShellPort()
{
	EjectShellPortAmmo();
}

void AWeapon::Handle_OnWeaponAnimInstanceWeaponHammer()
{
	if (HasAuthority())
	{
		if (PatronInWeaponAmmo)
		{
			PatronInWeaponAmmo->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, SHELL_PORT_SOCKET_NAME);
			PatronInWeaponAmmo->Server_SetIsEmpty(true);
			ShellPortAmmo = PatronInWeaponAmmo;
			PatronInWeaponAmmo = nullptr;
		}
	}
}

void AWeapon::Handle_OnWeaponAnimInstanceWeaponSelector()
{
	if (HasAuthority())
	{
		Server_SwitchFiremode();
	}
}
