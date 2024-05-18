#pragma once

#include "CoreMinimal.h"
#include "LeanDirection.generated.h"

UENUM(BlueprintType)
enum class ELeanDirection : uint8
{
	LD_None UMETA(DisplayName = "None"),
	LD_Left UMETA(DisplayName = "Left"),
	LD_Right UMETA(DisplayName = "Right"),
	Default_MAX UMETA(Hidden)
};