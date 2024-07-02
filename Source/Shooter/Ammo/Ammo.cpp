// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo.h"
#include "Net/UnrealNetwork.h"
#include "Shooter/Data/AmmoDataAsset.h"

AAmmo::AAmmo()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());

}

void AAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAmmo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAmmo, bIsEmpty);

}

void AAmmo::SetIsEmpty(bool bEmpty)
{
	bIsEmpty = bEmpty;
	if (Mesh && AmmoDataAsset)
	{
		Mesh->SetSkeletalMesh(bEmpty ? AmmoDataAsset->ShellMesh : AmmoDataAsset->FullMesh);
	}
	Server_SetIsEmpty(bEmpty);
}

void AAmmo::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAmmo::Server_SetIsEmpty_Implementation(bool bEmpty)
{
	bIsEmpty = bEmpty;
	/*if (Mesh && AmmoDataAsset)
	{
		Mesh->SetSkeletalMesh(bEmpty ? AmmoDataAsset->ShellMesh : AmmoDataAsset->FullMesh);
	}*/
}
