// Fill out your copyright notice in the Description page of Project Settings.


#include "Mod.h"

AMod::AMod()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());

}

void AMod::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMod::BeginPlay()
{
	Super::BeginPlay();
	
}

