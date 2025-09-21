// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Mod/Mod.h"
#include "Scope.generated.h"

UCLASS()
class SHOOTER_API AScope : public AMod
{
	GENERATED_BODY()

public:
	AScope();
	virtual void PostInitializeComponents() override;
	virtual FName GetDefaultAttachParentSocketName() const override;

protected:
	virtual bool HasModes() const;
	virtual uint8 GetNumModes() const;
	virtual void SetMode(uint8 Mode);
	TArray<FName> GetModeSocketNames() const;
	
};
