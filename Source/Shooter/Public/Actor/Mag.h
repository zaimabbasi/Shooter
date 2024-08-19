// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Mod.h"
#include "Mag.generated.h"

class AAmmo;
class UMagDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMagAmmoRemovedSignature, AAmmo*, RemovedAmmo);

UCLASS()
class SHOOTER_API AMag: public AMod
{
	GENERATED_BODY()
	
public:	
	AMag();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void AddAmmo(const uint8 Count);
	void RemoveAmmo();

	FOnMagAmmoRemovedSignature OnMagAmmoRemoved;

private:
	UFUNCTION(Server, Reliable)
	void Server_AddAmmo(const uint8 Count);

	UFUNCTION(Server, Reliable)
	void Server_RemoveAmmo();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataAsset", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UMagDataAsset> MagDataAsset;

	UPROPERTY(Replicated)
	uint8 AmmoCount;

	TMap<FName, TObjectPtr<AAmmo>> AmmoMap;

public:
	FORCEINLINE TSoftObjectPtr<UMagDataAsset> GetMagDataAsset() const { return MagDataAsset; }
	FORCEINLINE uint8 GetAmmoCount() const { return AmmoCount; }
	uint8 GetAmmoCapacity();
	uint8 GetAmmoSpace();

};
