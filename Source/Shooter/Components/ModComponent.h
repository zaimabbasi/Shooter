// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Shooter/Data/ModData.h"
#include "Shooter/Data/ModDataAsset.h"
#include "ModComponent.generated.h"

class AMod;
class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UModComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	friend class AWeapon;
	friend class AMod;

	UModComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

private:
	AActor* OwningActor;

	USkeletalMeshComponent* OwningActorMesh;

	UPROPERTY(Replicated)
	TArray<AMod*> ModArray;

	TArray<FModData> ModDataArray;

};
