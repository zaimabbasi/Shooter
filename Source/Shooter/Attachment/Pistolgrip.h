// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pistolgrip.generated.h"

UCLASS()
class SHOOTER_API APistolgrip : public AActor
{
	GENERATED_BODY()
	
public:	
	APistolgrip();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:

};
