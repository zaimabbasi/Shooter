// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ModComponent.generated.h"

class AMod;
class UModDataAsset;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UModComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UModComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Init(const UModDataAsset* ModDataAsset);

	template<class T>
	T* GetMod() const
	{
		T* Mod = nullptr;
		ModArray.FindItemByClass(&Mod);
		return Mod;
	};

protected:
	virtual void BeginPlay() override;	
	
private:
	UPROPERTY(Replicated)
	TArray<TObjectPtr<AMod>> ModArray;

};
