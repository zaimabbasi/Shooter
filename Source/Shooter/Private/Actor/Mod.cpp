// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Mod.h"
#include "ActorComponent/ModComponent.h"
#include "DataAsset/ModDataAsset.h"
#include "Type/ShooterNameType.h"

AMod::AMod()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	ModComponent = CreateDefaultSubobject<UModComponent>(TEXT("ModComponent"));
	ModComponent->SetIsReplicated(true);

}

FName AMod::GetDefaultAttachParentSocketName() const
{
	return NAME_None;
}

void AMod::Init()
{
	if (ModComponent)
	{
		ModComponent->Init(ModDataAsset.LoadSynchronous());
	}
}

void AMod::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void AMod::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMod::BeginPlay()
{
	Super::BeginPlay();
	
}

