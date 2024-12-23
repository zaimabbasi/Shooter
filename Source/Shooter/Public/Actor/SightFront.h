// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Mod.h"
#include "SightFront.generated.h"

UCLASS()
class SHOOTER_API ASightFront : public AMod
{
	GENERATED_BODY()
	
public:	
	ASightFront();
	virtual FName GetDefaultAttachParentSocketName() const override;

};
