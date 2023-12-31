// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class UBoxComponent;

UCLASS()
class SHOOTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxComponent;

public:
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }

};
