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

	UFUNCTION(Server, Reliable)
	void Server_SetIsEmpty(bool bEmpty);

	virtual void Tick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataAsset", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UAmmoDataAsset> AmmoDataAsset;

	UPROPERTY(ReplicatedUsing = OnRep_IsEmpty)
	bool bIsEmpty;

private:
	UFUNCTION()
	void OnRep_IsEmpty();

public:
	FORCEINLINE bool GetIsEmpty() const { return bIsEmpty; }
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }

};
