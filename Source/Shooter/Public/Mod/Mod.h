// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Mod.generated.h"

UCLASS()
class SHOOTER_API AMod : public AActor
{
	GENERATED_BODY()
	
public:
	AMod();
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;
	
	virtual void Init();
	virtual FName GetDefaultAttachParentSocketName() const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

public:
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }

};
