// Fill out your copyright notice in the Description page of Project Settings.


#include "Attachment.h"

AAttachment::AAttachment()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(GetRootComponent());

}

void AAttachment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAttachment::BeginPlay()
{
	Super::BeginPlay();
	
}

