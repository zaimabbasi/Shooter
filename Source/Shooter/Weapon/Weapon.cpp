// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/BoxComponent.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(GetRootComponent());

}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

}

