// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Reciever.generated.h"

UCLASS()
class SHOOTER_API AReciever : public AActor
{
	GENERATED_BODY()
	
public:	
	AReciever();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:

};
