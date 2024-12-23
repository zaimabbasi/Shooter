// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Mod.h"
#include "Tactical.generated.h"

UCLASS()
class SHOOTER_API ATactical : public AMod
{
	GENERATED_BODY()
	
public:
	ATactical();
	virtual FName GetDefaultAttachParentSocketName() const override;

};
