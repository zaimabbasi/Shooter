// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Ammo.h"
#include "Net/UnrealNetwork.h"
#include "DataAsset/AmmoDataAsset.h"

AAmmo::AAmmo()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

}

void AAmmo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAmmo, bIsEmpty);

}

void AAmmo::Server_SetIsEmpty_Implementation(bool bEmpty)
{
	bIsEmpty = bEmpty;
	UAmmoDataAsset* LoadedAmmoDataAsset = AmmoDataAsset.LoadSynchronous();
	if (Mesh && LoadedAmmoDataAsset)
	{
		Mesh->SetSkeletalMesh(bEmpty ? LoadedAmmoDataAsset->ShellMesh : LoadedAmmoDataAsset->FullMesh);
	}
}

void AAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAmmo::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAmmo::OnRep_IsEmpty()
{
	UAmmoDataAsset* LoadedAmmoDataAsset = AmmoDataAsset.LoadSynchronous();
	if (Mesh && LoadedAmmoDataAsset)
	{
		Mesh->SetSkeletalMesh(bIsEmpty ? LoadedAmmoDataAsset->ShellMesh : LoadedAmmoDataAsset->FullMesh);
	}
}
