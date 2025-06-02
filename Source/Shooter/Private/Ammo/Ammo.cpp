// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo/Ammo.h"
#include "Net/UnrealNetwork.h"
#include "Data/AmmoDataAsset.h"

AAmmo::AAmmo() :
	bIsEmpty(false)
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

}

void AAmmo::BeginPlay()
{
	Super::BeginPlay();

}

//void AAmmo::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

void AAmmo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AAmmo, bIsEmpty, COND_SimulatedOnly);

}

void AAmmo::SetIsEmpty(bool bEmpty)
{
	bIsEmpty = bEmpty;

	UpdateMesh();
}

void AAmmo::OnRep_IsEmpty()
{
	UpdateMesh();
}

void AAmmo::UpdateMesh() const
{
	if (Mesh && AmmoDataAsset.LoadSynchronous())
	{
		Mesh->SetSkeletalMesh(bIsEmpty ? AmmoDataAsset->ShellMesh : AmmoDataAsset->FullMesh);
	}
}
