// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ammo.generated.h"

class UAmmoDataAsset;

UCLASS()
class SHOOTER_API AAmmo : public AActor
{
	GENERATED_BODY()
	
public:	
	AAmmo();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void SetIsEmpty(bool bEmpty);

protected:
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_SetIsEmpty(bool bEmpty);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;

	UPROPERTY(Replicated)
	bool bIsEmpty;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataAsset", meta = (AllowPrivateAccess = "true"))
	UAmmoDataAsset* AmmoDataAsset;

public:
	FORCEINLINE bool GetIsEmpty() const { return bIsEmpty; }

};
