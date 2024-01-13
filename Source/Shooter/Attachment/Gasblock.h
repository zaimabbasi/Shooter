// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gasblock.generated.h"

UCLASS()
class SHOOTER_API AGasblock : public AActor
{
	GENERATED_BODY()
	
public:	
	AGasblock();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:

};
