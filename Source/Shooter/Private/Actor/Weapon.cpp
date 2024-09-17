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
#include "DataAsset/MagDataAsset.h"
#include "DataAsset/ModDataAsset.h"
#include "DataAsset/WeaponDataAsset.h"
#include "Type/ShooterNameType.h"

AWeapon::AWeapon() :
	CombatAction(ECombatAction::CA_Out)
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

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, PatronInWeapon);
	DOREPLIFETIME(AWeapon, bIsHolster);

}

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Mesh)
	{
		if (UWeaponAnimInstance* WeaponAnimInstance = Cast<UWeaponAnimInstance>(Mesh->GetAnimInstance()))
		{
			WeaponAnimInstance->OnWeaponAnimInstanceIdle.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceIdle);
			WeaponAnimInstance->OnWeaponAnimInstanceIdleToOut.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceIdleToOut);
			WeaponAnimInstance->OnWeaponAnimInstanceOut.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceOut);
			WeaponAnimInstance->OnWeaponAnimInstanceOutToIdle.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceOutToIdle);
			WeaponAnimInstance->OnWeaponAnimInstancePatronInWeapon.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstancePatronInWeapon);
		}
	}

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

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

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

void AWeapon::SetCombatAction(ECombatAction Action)
{
	CombatAction = Action;
}

bool AWeapon::DoesNeedCharge()
{
	return (GetMagAmmoCount() > 0 && PatronInWeapon == nullptr);
}

bool AWeapon::CanFire()
{
	return (CombatAction == ECombatAction::CA_Idle && PatronInWeapon);
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
		PoppedAmmo->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, WEAPON_PATRON_IN_WEAPON_SOCKET_NAME);
	}
	PatronInWeapon = PoppedAmmo;
}

void AWeapon::Handle_OnWeaponAnimInstanceIdle()
{
	OnWeaponIdle.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceIdleToOut()
{
	OnWeaponIdleToOut.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceOut()
{
	OnWeaponOut.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceOutToIdle()
{
	OnWeaponOutToIdle.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstancePatronInWeapon()
{
	if (HasAuthority())
	{
		Server_MagPopAmmo();
	}
}
