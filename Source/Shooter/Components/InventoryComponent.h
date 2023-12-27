// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class AShooterCharacter;
class AWeapon;

DECLARE_DELEGATE(FOnRepWeaponsArrayDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	friend class AShooterCharacter;

	UInventoryComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	FOnRepWeaponsArrayDelegate OnRepWeaponsArrayDelegate;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_WeaponsArray();

private:
	AShooterCharacter* OwningCharacter;

	UPROPERTY(ReplicatedUsing = "OnRep_WeaponsArray")
	TArray<AWeapon*> WeaponsArray;

public:
	FORCEINLINE bool HasAuthority() const
	{
		APawn* OwningPawn = GetOwner<APawn>();
		return OwningPawn && OwningPawn->HasAuthority();
	}
	FORCEINLINE bool IsLocallyControlled() const
	{
		APawn* OwningPawn = GetOwner<APawn>();
		return OwningPawn && OwningPawn->IsLocallyControlled();
	}
};
