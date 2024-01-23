// Fill out your copyright notice in the Description page of Project Settings.


#include "HandsAnimInstance.h"
#include "ShooterCharacter.h"

void UHandsAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UHandsAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}
	if (ShooterCharacter == nullptr)
	{
		return;
	}

	if (USkeletalMeshComponent* CharacterMesh = ShooterCharacter->GetMesh())
	{
		FVector CharacterBaseHumanRibcageLocation = CharacterMesh->GetBoneLocation(TEXT("Base-HumanRibcage"), EBoneSpaces::ComponentSpace);
		if (USkeletalMeshComponent* HandsMesh = ShooterCharacter->GetHandsMesh())
		{
			HandsMesh->SetRelativeLocation(CharacterBaseHumanRibcageLocation);
		}
	}

}
