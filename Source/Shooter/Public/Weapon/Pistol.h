// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "Pistol.generated.h"

UCLASS()
class SHOOTER_API APistol : public AWeapon
{
	GENERATED_BODY()
	
public:
	APistol();
	virtual bool IsPistol() const override { return true; }

protected:
	virtual void OnInventoryIndexChanged() override;

};
