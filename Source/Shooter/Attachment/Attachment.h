// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Attachment.generated.h"

class AWeapon;
class UBoxComponent;

UCLASS()
class SHOOTER_API AAttachment : public AActor
{
	GENERATED_BODY()
	
public:	
	AAttachment();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	AWeapon* OwningWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;

public:
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }

};
