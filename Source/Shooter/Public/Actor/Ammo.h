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
	virtual void Tick(float DeltaTime) override;

	void SetIsEmpty(bool bEmpty);
	
protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnRep_IsEmpty();

	void UpdateMesh() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataAsset", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UAmmoDataAsset> AmmoDataAsset;

	UPROPERTY(ReplicatedUsing = OnRep_IsEmpty)
	bool bIsEmpty;

public:
	FORCEINLINE bool GetIsEmpty() const { return bIsEmpty; }
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }

};
