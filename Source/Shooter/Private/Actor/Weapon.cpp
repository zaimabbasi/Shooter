// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Weapon.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "Actor/Ammo.h"
#include "Actor/Mag.h"
#include "ActorComponent/ModComponent.h"
#include "AnimInstance/WeaponAnimInstance.h"
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

	WeaponAction = EWeaponAction::WAS_OutToIdle;

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

	if (Mesh)
	{
		AnimClass = Mesh->GetAnimClass();
		Mesh->SetAnimClass(nullptr);
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

void AWeapon::SetAnimInstance()
{
	if (Mesh == nullptr)
	{
		return;
	}
	Mesh->SetAnimClass(AnimClass);
}

void AWeapon::ClearAnimInstance()
{
	if (Mesh == nullptr)
	{
		return;
	}
	Mesh->SetAnimClass(nullptr);
}

void AWeapon::SetAnimInstanceDelegateBindings()
{
	if (Mesh == nullptr)
	{
		return;
	}
	if (UWeaponAnimInstance* WeaponAnimInstance = Cast<UWeaponAnimInstance>(Mesh->GetAnimInstance()))
	{
		WeaponAnimInstance->OnWeaponAnimInstanceIdle.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceIdle);
		WeaponAnimInstance->OnWeaponAnimInstanceIdleToOut.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceIdleToOut);
		WeaponAnimInstance->OnWeaponAnimInstanceOut.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceOut);
		WeaponAnimInstance->OnWeaponAnimInstanceOutToIdle.AddDynamic(this, &AWeapon::Handle_OnWeaponAnimInstanceOutToIdle);
	}
}

void AWeapon::ClearAnimInstanceDelegateBindings()
{
	if (Mesh == nullptr)
	{
		return;
	}
	if (UWeaponAnimInstance* WeaponAnimInstance = Cast<UWeaponAnimInstance>(Mesh->GetAnimInstance()))
	{
		WeaponAnimInstance->OnWeaponAnimInstanceIdle.RemoveAll(this);
		WeaponAnimInstance->OnWeaponAnimInstanceIdleToOut.RemoveAll(this);
		WeaponAnimInstance->OnWeaponAnimInstanceOut.RemoveAll(this);
		WeaponAnimInstance->OnWeaponAnimInstanceOutToIdle.RemoveAll(this);
	}
}

void AWeapon::SetWeaponAction(EWeaponAction NewWeaponAction)
{
	if (!HasAuthority())
	{
		WeaponAction = NewWeaponAction;
	}
	Server_SetWeaponAction(NewWeaponAction);
}

void AWeapon::Server_SetWeaponAction_Implementation(EWeaponAction NewWeaponAction)
{
	WeaponAction = NewWeaponAction;
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
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));
	OnWeaponIdle.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceIdleToOut()
{
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));
	OnWeaponIdleToOut.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceOut()
{
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));
	OnWeaponOut.Broadcast(this);
}

void AWeapon::Handle_OnWeaponAnimInstanceOutToIdle()
{
	UE_LOG(LogTemp, Warning, TEXT(__FUNCTION__));
	OnWeaponOutToIdle.Broadcast(this);
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
