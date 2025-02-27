#pragma once
#include "CoreMinimal.h"
#include "CharacterTypes.generated.h"

UENUM(BlueprintType)
enum class ELeaningDirection : uint8
{
	LD_None UMETA(DisplayName = "None"),
	LD_Left UMETA(DisplayName = "Left"),
	LD_Right UMETA(DisplayName = "Right"),
	Default_MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ETurningDirection : uint8
{
	TD_None UMETA(DisplayName = "None"),
	TD_Left UMETA(DisplayName = "Left"),
	TD_Right UMETA(DisplayName = "Right"),
	Default_MAX UMETA(Hidden)
};