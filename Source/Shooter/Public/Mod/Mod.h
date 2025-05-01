// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Mod.generated.h"

class UWeaponModComponent;
class UModDataAsset;

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

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UModDataAsset> ModDataAsset;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWeaponModComponent> WeaponModComponent;

public:
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }

};
