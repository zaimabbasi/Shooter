// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Mod.generated.h"

class UModComponent;
class UModDataAsset;

UCLASS()
class SHOOTER_API AMod : public AActor
{
	GENERATED_BODY()
	
public:
	AMod();
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	
protected:
	virtual void BeginPlay() override;
	
public:
	virtual void Init();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ActorComponent", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UModComponent> ModComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DataAsset", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UModDataAsset> ModDataAsset;

public:
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }

};
