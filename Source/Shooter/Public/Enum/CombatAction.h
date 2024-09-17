#pragma once

#include "CoreMinimal.h"
#include "CombatAction.generated.h"

UENUM(BlueprintType)
enum class ECombatAction : uint8
{
	CA_None UMETA(DisplayName = "None"),
	CA_Idle UMETA(DisplayName = "Idle"),
	CA_IdleToOut UMETA(DisplayName = "IdleToOut"),
	CA_Out UMETA(DisplayName = "Out"),
	CA_OutToIdle UMETA(DisplayName = "OutToIdle"),
	CA_OutToIdleArm UMETA(DisplayName = "OutToIdleArm"),
	Default_MAX UMETA(Hidden)
};
