// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Weapon.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "Actor/Ammo.h"
#include "Actor/Mag.h"
#include "ActorComponent/ModComponent.h"
#include "DataAsset/MagDataAsset.h"
#include "DataAsset/ModDataAsset.h"
#include "DataAsset/WeaponDataAsset.h"

AWeapon::AWeapon()
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

	DOREPLIFETIME(AWeapon, WeaponAction);

}

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

bool AWeapon::HandleAnimNotify(const FAnimNotifyEvent& AnimNotifyEvent)
{
	return true;
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
			Mag->OnAmmoRemovedDelegate.AddDynamic(this, &AWeapon::Handle_OnAmmoRemoved);
		}
	}
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

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

}

void AWeapon::Handle_OnAmmoRemoved(AAmmo* RemovedAmmo)
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
	RemovedAmmo->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform, TEXT("patron_in_weapon"));
	AmmoInChamber = RemovedAmmo;
}

UClass* AWeapon::GetHandsAnimClass() const
{
	if (WeaponDataAsset.IsNull())
	{
		return nullptr;
	}
	return WeaponDataAsset.LoadSynchronous()->HandsAnimClass;
}

AMag* AWeapon::GetMag()
{
	if (ModComponent == nullptr)
	{
		return nullptr;
	}
	return ModComponent->GetMag();
}
