// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Scope.h"
#include "MagnifyingScope.generated.h"

class USceneCaptureComponent2D;

UCLASS()
class SHOOTER_API AMagnifyingScope : public AScope
{
	GENERATED_BODY()
	
public:
	AMagnifyingScope();
	virtual FName GetDefaultAttachParentSocketName() const override;
	virtual void PostInitializeComponents() override;

private:
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> LensMesh;*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneCaptureComponent2D> OpticCamera;

};
