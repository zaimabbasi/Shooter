// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ModComponent.generated.h"

class ABarrel;
class ACharge;
//class AFlashlight;
//class AForegrip;
class AGasBlock;
class AHandguard;
class AMag;
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
class UModDataAsset;

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

	UModDataAsset* ModDataAsset;

	UPROPERTY(Replicated)
	ABarrel* Barrel;

	UPROPERTY(Replicated)
	ACharge* Charge;

	UPROPERTY(Replicated)
	AGasBlock* GasBlock;

	UPROPERTY(Replicated)
	AHandguard* Handguard;

	UPROPERTY(Replicated)
	AMag* Mag;

	UPROPERTY(Replicated)
	AMuzzle* Muzzle;

	UPROPERTY(Replicated)
	APistolgrip* Pistolgrip;

	UPROPERTY(Replicated)
	AReciever* Reciever;

	UPROPERTY(Replicated)
	ASightFront* SightFront;

	UPROPERTY(Replicated)
	ASightRear* SightRear;

	UPROPERTY(Replicated)
	AStock* Stock;

	void AttachModToOwningActorSocket(AMod* Mod, FName ModSocketName);

public:
	FORCEINLINE AMag* GetMag() const { return Mag; }

};
