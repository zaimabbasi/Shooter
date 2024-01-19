// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Shooter/Data/WeaponDataAsset.h"
#include "ModComponent.generated.h"

class ABarrel;
//class AFlashlight;
//class AForegrip;
class AGasblock;
class AHandguard;
class AMagazine;
//class AMount;
class AMuzzle;
class APistolgrip;
class AReciever;
//class AScope;
class ASightFront;
class ASightRear;
class AStock;
//class ATactical;
class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UModComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	friend class AWeapon;

	UModComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

private:
	AWeapon* OwningWeapon;

	FModData ModData;

	UPROPERTY(Replicated)
	ABarrel* Barrel;

	/*UPROPERTY(Replicated)
	AFlashlight* Flashlight;*/

	/*UPROPERTY(Replicated)
	AForegrip* Foregrip;*/

	UPROPERTY(Replicated)
	AGasblock* Gasblock;

	UPROPERTY(Replicated)
	AHandguard* Handguard;

	UPROPERTY(Replicated)
	AMagazine* Magazine;

	/*UPROPERTY(Replicated)
	AMount* Mount;*/

	UPROPERTY(Replicated)
	AMuzzle* Muzzle;

	UPROPERTY(Replicated)
	APistolgrip* Pistolgrip;

	UPROPERTY(Replicated)
	AReciever* Reciever;

	/*UPROPERTY(Replicated)
	AScope* Scope;*/

	UPROPERTY(Replicated)
	ASightFront* SightFront;

	UPROPERTY(Replicated)
	ASightRear* SightRear;

	UPROPERTY(Replicated)
	AStock* Stock;

	/*UPROPERTY(Replicated)
	ATactical* Tactical;*/

};
