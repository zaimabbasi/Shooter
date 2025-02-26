// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Mod/Mod.h"
#include "Stock.generated.h"

UCLASS()
class SHOOTER_API AStock : public AMod
{
	GENERATED_BODY()
	
public:	
	AStock();
	virtual FName GetDefaultAttachParentSocketName() const override;

};
