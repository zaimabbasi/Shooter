// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Mod/Mod.h"
#include "Barrel.generated.h"

UCLASS()
class SHOOTER_API ABarrel : public AMod
{
	GENERATED_BODY()
	
public:	
	ABarrel();
	virtual FName GetDefaultAttachParentSocketName() const override;

};
