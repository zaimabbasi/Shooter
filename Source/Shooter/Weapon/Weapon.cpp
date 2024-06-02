// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "Shooter/Components/ModComponent.h"
#include "Shooter/Data/WeaponDataAsset.h"

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

	if (ModComponent)
	{
		ModComponent->OwningWeapon = this;
		if (WeaponDataAsset)
		{
			ModComponent->ModData = WeaponDataAsset->ModData;
		}
	}

}

bool AWeapon::HandleAnimNotify(const FAnimNotifyEvent& AnimNotifyEvent)
{
	return true;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

}

void AWeapon::SetActorHiddenInGameWithChildren(bool bNewHidden)
{
	SetActorHiddenInGame(bNewHidden);
	for (AActor* ChildActor : Children)
	{
		ChildActor->SetActorHiddenInGame(bNewHidden);
	}
}

UClass* AWeapon::GetHandsAnimClass() const
{
	if (WeaponDataAsset == nullptr)
	{
		return nullptr;
	}
	return WeaponDataAsset->HandsAnimClass;
}

USkeletalMeshComponent* AWeapon::GetAimCameraSocketParentMesh()
{
	if (ModComponent == nullptr)
	{
		return nullptr;
	}
	return ModComponent->AimCameraSocketParentMesh;
}

FName AWeapon::GetAimCameraSocketName()
{
	if (ModComponent == nullptr)
	{
		return NAME_None;
	}
	return ModComponent->AimCameraSocketName;
}

