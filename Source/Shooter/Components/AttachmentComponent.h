// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Shooter/Data/WeaponDataAsset.h"
#include "AttachmentComponent.generated.h"

class ABarrel;
class AFlashlight;
class AForegrip;
class AGasblock;
class AHandguard;
class AMagazine;
class AMount;
class AMuzzle;
class APistolgrip;
class AReciever;
class AScope;
class ASightFront;
class ASightRear;
class AStock;
class ATactical;
class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UAttachmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	friend class AWeapon;

	UAttachmentComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:
	AWeapon* OwningWeapon;

	FAttachmentData AttachmentData;

	ABarrel* Barrel;
	AFlashlight* Flashlight;
	AForegrip* Foregrip;
	AGasblock* Gasblock;
	AHandguard* Handguard;
	AMagazine* Magazine;
	AMount* Mount;
	AMuzzle* Muzzle;
	APistolgrip* Pistolgrip;
	AReciever* Reciever;
	AScope* Scope;
	ASightFront* SightFront;
	ASightRear* SightRear;
	AStock* Stock;
	ATactical* Tactical;

};
