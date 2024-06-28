// Fill out your copyright notice in the Description page of Project Settings.


#include "Mod.h"
#include "Shooter/Components/ModComponent.h"
#include "Shooter/Data/ModDataAsset.h"

AMod::AMod()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());

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
		ModComponent->OwningActorMesh = Mesh;
		if (ModDataAsset)
		{
			ModComponent->ModDataArray = ModDataAsset->ModDataArray;
		}
	}
}

void AMod::SetModType(EModType Type)
{
	ModType = Type;
}

void AMod::BeginPlay()
{
	Super::BeginPlay();
	
}

