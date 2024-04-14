// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/BoxComponent.h"
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

	if (WeaponDataAsset)
	{
		HandsAnimationDataAsset = WeaponDataAsset->HandsAnimationDataAsset;
		WeaponAnimationDataAsset = WeaponDataAsset->WeaponAnimationDataAsset;
	}
}

void AWeapon::SetActorHiddenInGameWithChildren(bool bNewHidden)
{
	SetActorHiddenInGame(bNewHidden);
	for (AActor* ChildActor : Children)
	{
		ChildActor->SetActorHiddenInGame(bNewHidden);
	}
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

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

