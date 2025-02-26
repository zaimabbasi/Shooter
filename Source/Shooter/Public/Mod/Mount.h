// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Mod/Mod.h"
#include "Mount.generated.h"

UCLASS()
class SHOOTER_API AMount : public AMod
{
	GENERATED_BODY()
	
public:
	AMount();
	virtual FName GetDefaultAttachParentSocketName() const override;

};
