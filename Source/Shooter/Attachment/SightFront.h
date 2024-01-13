// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SightFront.generated.h"

UCLASS()
class SHOOTER_API ASightFront : public AActor
{
	GENERATED_BODY()
	
public:	
	ASightFront();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:

};
