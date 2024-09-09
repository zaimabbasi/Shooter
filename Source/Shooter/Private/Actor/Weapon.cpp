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
			Mag->OnMagAmmoRemoved.AddDynamic(this, &AWeapon::Handle_OnMagAmmoRemoved);
		}
	}
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

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

void AWeapon::LoadAmmoInChamber()
{
	if (ModComponent == nullptr)
	{
		return;
	}
	if (!AmmoInChamber)
	{
		if (AMag* Mag = ModComponent->GetMag())
		{
			Mag->RemoveAmmo();
		}
	}
}

void AWeapon::SetCombatAction(ECombatAction Action)
{
	CombatAction = Action;
}

void AWeapon::Server_SetIsHolster_Implementation(const bool bHolster)
{
	bIsHolster = bHolster;
}

void AWeapon::Handle_OnMagAmmoRemoved(AAmmo* RemovedAmmo)
{
	if (RemovedAmmo == nullptr)
	{
		if (AMag* Mag = GetMag())
		{
			if (const UMagDataAsset* MagDataAsset = Mag->GetMagDataAsset().LoadSynchronous())
			{
				if (UWorld* World = GetWorld())
				{
					RemovedAmmo = World->SpawnActor<AAmmo>(MagDataAsset->AmmoClass);
				}
			}
		}
	}
	RemovedAmmo->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform, GetPatronInWeaponSocketName());
	AmmoInChamber = RemovedAmmo;
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

AMag* AWeapon::GetMag()
{
	if (ModComponent == nullptr)
	{
		return nullptr;
	}
	return ModComponent->GetMag();
}
