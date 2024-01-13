// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Stock.generated.h"

UCLASS()
class SHOOTER_API AStock : public AActor
{
	GENERATED_BODY()
	
public:	
	AStock();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:

};
