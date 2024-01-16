// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/BoxComponent.h"
#include "Shooter/Components/AttachmentComponent.h"
#include "Shooter/Data/WeaponDataAsset.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(GetRootComponent());

	AttachmentComponent = CreateDefaultSubobject<UAttachmentComponent>(TEXT("AttachmentComponent"));
	AttachmentComponent->SetIsReplicated(true);

}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (AttachmentComponent)
	{
		AttachmentComponent->OwningWeapon = this;
		if (WeaponDataAsset)
		{
			AttachmentComponent->AttachmentData = WeaponDataAsset->AttachmentData;
		}
	}
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

}

