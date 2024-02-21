#pragma once

#include "CoreMinimal.h"

UENUM()
enum class ETurnInPlace : uint8
{
	TIP_None UMETA(DisplayName = "None"),
	TIP_Left UMETA(DisplayName = "Left"),
	TIP_Right UMETA(DisplayName = "Right"),
	Default_MAX
};