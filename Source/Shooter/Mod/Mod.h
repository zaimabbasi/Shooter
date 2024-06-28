// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shooter/Types/ModType.h"
#include "Mod.generated.h"

class AWeapon;
class UModComponent;
class UModDataAsset;

UCLASS()
class SHOOTER_API AMod : public AActor
{
	GENERATED_BODY()
	
public:	
	AMod();
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	void SetModType(EModType Type);
	
protected:
	virtual void BeginPlay() override;

	//AWeapon* OwningWeapon;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataAsset", meta = (AllowPrivateAccess = "true"))
	UModDataAsset* ModDataAsset;

	EModType ModType;

	UModComponent* ModComponent;

public:
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }
	FORCEINLINE EModType GetModType() const { return ModType; }

};
