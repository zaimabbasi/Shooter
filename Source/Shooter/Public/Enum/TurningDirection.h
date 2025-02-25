#pragma once

#include "CoreMinimal.h"
#include "TurningDirection.generated.h"

UENUM(BlueprintType)
enum class ETurningDirection : uint8
{
	TD_None UMETA(DisplayName = "None"),
	TD_Left UMETA(DisplayName = "Left"),
	TD_Right UMETA(DisplayName = "Right"),
	Default_MAX UMETA(Hidden)
};