// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Mod/Mod.h"
#include "Muzzle.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class SHOOTER_API AMuzzle : public AMod
{
	GENERATED_BODY()
	
public:	
	AMuzzle();
	virtual FName GetDefaultAttachParentSocketName() const override;
	virtual void SpawnMuzzleFlashEffect(UNiagaraSystem* MuzzleFlashSystem) const;
	//virtual UNiagaraComponent* SpawnMuzzleSmokeEffect(UNiagaraSystem* MuzzleSmokeSystem) const;
	FTransform GetMuzzleFlashSocketTransform() const;

};
