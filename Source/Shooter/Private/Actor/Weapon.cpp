// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Weapon.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "ActorComponent/ModComponent.h"
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

	if (ModComponent)
	{
		ModComponent->OwningActor = this;
		ModComponent->ModDataAsset = ModDataAsset;
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

UClass* AWeapon::GetHandsAnimClass() const
{
	if (WeaponDataAsset == nullptr)
	{
		return nullptr;
	}
	return WeaponDataAsset->HandsAnimClass;
}

AMag* AWeapon::GetMag()
{
	if (ModComponent == nullptr)
	{
		return nullptr;
	}
	return ModComponent->GetMag();
}
