#pragma once

#include "CoreMinimal.h"
#include "TurnDirection.generated.h"

UENUM()
enum class ETurnDirection : uint8
{
	TD_None UMETA(DisplayName = "None"),
	TD_Left UMETA(DisplayName = "Left"),
	TD_Right UMETA(DisplayName = "Right"),
	Default_MAX
};