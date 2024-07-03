// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Mod.h"
#include "Mag.generated.h"

class AAmmo;
class UMagDataAsset;

UCLASS()
class SHOOTER_API AMag: public AMod
{
	GENERATED_BODY()
	
public:	
	AMag();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void AddAmmo(uint8 Count);

protected:
	UFUNCTION(Server, Reliable)
	void Server_AddAmmo(uint8 Count);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataAsset", meta = (AllowPrivateAccess = "true"))
	UMagDataAsset* MagDataAsset;

	UPROPERTY(Replicated)
	uint8 AmmoCount;

	TMap<FName, AAmmo*> AmmoMap;

public:
	FORCEINLINE uint8 GetAmmoCount() const { return AmmoCount; }
	uint8 GetAmmoCapacity();
	uint8 GetAmmoSpace();

};
