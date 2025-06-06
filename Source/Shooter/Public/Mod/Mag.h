// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Mod/Mod.h"
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

	virtual void Init() override;
	virtual FName GetDefaultAttachParentSocketName() const override;
	uint8 GetAmmoCapacity() const;
	uint8 GetAmmoSpace() const;

	void AddAmmo(uint8 Count);
	AAmmo* PopAmmo();

protected:
	AAmmo* GetAmmoAtIndex(uint8 AmmoIndex) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UMagDataAsset> MagDataAsset;

	UPROPERTY(Replicated)
	uint8 AmmoCount;

	UPROPERTY(Replicated)
	TArray<TObjectPtr<AAmmo>> AmmoArray;

public:
	FORCEINLINE uint8 GetAmmoCount() const { return AmmoCount; }
	FORCEINLINE TSoftObjectPtr<UMagDataAsset> GetMagDataAsset() const { return MagDataAsset; }

};
