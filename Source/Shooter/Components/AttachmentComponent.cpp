// Fill out your copyright notice in the Description page of Project Settings.


#include "AttachmentComponent.h"

UAttachmentComponent::UAttachmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UAttachmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UAttachmentComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

