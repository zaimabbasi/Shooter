// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Mod.h"
#include "ActorComponent/ModComponent.h"
#include "DataAsset/ModDataAsset.h"

AMod::AMod()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	ModComponent = CreateDefaultSubobject<UModComponent>(TEXT("ModComponent"));
	ModComponent->SetIsReplicated(true);

}

void AMod::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMod::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (ModComponent)
	{
		ModComponent->OwningActor = this;
		ModComponent->ModDataAsset = ModDataAsset;
	}
}

void AMod::BeginPlay()
{
	Super::BeginPlay();
	
}

