// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Mod.h"
#include "GasBlock.generated.h"

UCLASS()
class SHOOTER_API AGasBlock : public AMod
{
	GENERATED_BODY()
	
public:	
	AGasBlock();
	virtual FName GetDefaultAttachParentSocketName() const override;

};
