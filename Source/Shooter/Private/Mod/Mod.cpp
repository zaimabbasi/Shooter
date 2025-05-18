// Fill out your copyright notice in the Description page of Project Settings.


#include "Mod/Mod.h"
#include "Components/WeaponModComponent.h"
#include "Data/ModDataAsset.h"
#include "Types/ShooterNames.h"

AMod::AMod()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	/*WeaponModComponent = CreateDefaultSubobject<UWeaponModComponent>(TEXT("ModComponent"));
	WeaponModComponent->SetIsReplicated(true);*/

}

void AMod::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void AMod::BeginPlay()
{
	Super::BeginPlay();

}

void AMod::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FName AMod::GetDefaultAttachParentSocketName() const
{
	return NAME_None;
}

//void AMod::Init()
//{
//	if (WeaponModComponent)
//	{
//		WeaponModComponent->Init(ModDataAsset.LoadSynchronous());
//	}
//}
