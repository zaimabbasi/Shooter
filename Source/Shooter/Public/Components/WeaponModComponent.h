// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponModComponent.generated.h"

class AMod;
class UWeaponModDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponModModArrayReplicatedSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UWeaponModComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponModComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//virtual void Init(const UModDataAsset* ModDataAsset);
	virtual void Init(const UWeaponModDataAsset* ModDataAsset);

	template<class T>
	T* GetMod() const
	{
		T* Mod = nullptr;
		ModArray.FindItemByClass(&Mod);
		return Mod;
	};

protected:
	virtual void BeginPlay() override;
	virtual AMod* SpawnMod(TSubclassOf<AMod> ModClass, AActor* ParentActor, FName AttachParentSocketName = NAME_None);

	UFUNCTION()
	virtual void OnRep_ModArray() const;
	
public:
	FOnWeaponModModArrayReplicatedSignature OnWeaponModModArrayReplicated;

private:
	UPROPERTY(ReplicatedUsing = OnRep_ModArray)
	TArray<TObjectPtr<AMod>> ModArray;

};
