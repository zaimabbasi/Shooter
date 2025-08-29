// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Mod/Scope.h"
#include "MagnifyingScope.generated.h"

class USceneCaptureComponent2D;

UCLASS()
class SHOOTER_API AMagnifyingScope : public AScope
{
	GENERATED_BODY()
	
public:
	AMagnifyingScope();
	virtual void PostInitializeComponents() override;

public:
	virtual FName GetDefaultAttachParentSocketName() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true", UIMin = "1", UIMax = "100", ClampMin = "1", ClampMax = "100", Units = "Times"))
	float Magnification;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneCaptureComponent2D> OpticCamera;

};
