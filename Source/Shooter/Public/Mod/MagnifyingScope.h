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
	virtual FName GetDefaultAttachParentSocketName() const override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneCaptureComponent2D> OpticCamera;

};
