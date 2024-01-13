// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Handguard.generated.h"

UCLASS()
class SHOOTER_API AHandguard : public AActor
{
	GENERATED_BODY()
	
public:	
	AHandguard();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:

};
