// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/Mod.h"
#include "Mag.generated.h"

class AAmmo;
class UMagDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMagAmmoPoppedSignature, AAmmo*, PoppedAmmo);

UCLASS()
class SHOOTER_API AMag: public AMod
{
	GENERATED_BODY()
	
public:	
	AMag();	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual FName GetDefaultAttachParentSocketName() const override;
	uint8 GetAmmoCapacity() const;

	void AddAmmo(const uint8 Count);
	void PopAmmo();

	FOnMagAmmoPoppedSignature OnMagAmmoPopped;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataAsset", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UMagDataAsset> MagDataAsset;

	UPROPERTY(Replicated)
	uint8 AmmoCount;

	UPROPERTY(Replicated)
	TArray<TObjectPtr<AAmmo>> AmmoArray;

public:
	FORCEINLINE uint8 GetAmmoCount() const { return AmmoCount; }
	FORCEINLINE TSoftObjectPtr<UMagDataAsset> GetMagDataAsset() const { return MagDataAsset; }

};
