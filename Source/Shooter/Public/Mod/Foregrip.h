// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Mod/Mod.h"
#include "Foregrip.generated.h"

UCLASS()
class SHOOTER_API AForegrip : public AMod
{
	GENERATED_BODY()
	
public:
	AForegrip();
	virtual FName GetDefaultAttachParentSocketName() const override;

};
