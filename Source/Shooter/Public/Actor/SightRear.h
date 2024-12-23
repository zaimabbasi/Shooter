// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Mod.h"
#include "SightRear.generated.h"

UCLASS()
class SHOOTER_API ASightRear : public AMod
{
	GENERATED_BODY()
	
public:	
	ASightRear();
	virtual FName GetDefaultAttachParentSocketName() const override;

};
