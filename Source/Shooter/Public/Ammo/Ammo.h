// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ammo.generated.h"

class UAmmoDataAsset;

UCLASS()
class SHOOTER_API AAmmo : public AActor
{
	GENERATED_BODY()
	
public:	
	AAmmo();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//virtual void Tick(float DeltaTime) override;

	virtual void SetIsEmpty(bool bEmpty);
	
protected:
	virtual void BeginPlay() override;
	
	virtual void UpdateMesh() const;

	/*UFUNCTION()
	virtual void OnRep_IsEmpty();*/

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UAmmoDataAsset> AmmoDataAsset;

	//UPROPERTY(ReplicatedUsing = OnRep_IsEmpty)
	bool bIsEmpty;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

public:
	FORCEINLINE bool GetIsEmpty() const { return bIsEmpty; }
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }

};
