#pragma once

#include "CoreMinimal.h"
#include "CombatAction.generated.h"

UENUM(BlueprintType)
enum class ECombatAction : uint8
{
	CA_None UMETA(DisplayName = "None"),
	CA_ActionEnd UMETA(DisplayName = "ActionEnd"),
	CA_ActionStart UMETA(DisplayName = "ActionStart"),
	CA_ChamberCheck UMETA(DisplayName = "ChamberCheck"),
	CA_Fire UMETA(DisplayName = "Fire"),
	CA_Firemode UMETA(DisplayName = "Firemode"),
	CA_FiremodeCheck UMETA(DisplayName = "FiremodeCheck"),
	CA_Idle UMETA(DisplayName = "Idle"),
	CA_IdleToOut UMETA(DisplayName = "IdleToOut"),
	CA_MagCheck UMETA(DisplayName = "MagCheck"),
	CA_MagIn UMETA(DisplayName = "MagIn"),
	CA_MagOut UMETA(DisplayName = "MagOut"),
	CA_Out UMETA(DisplayName = "Out"),
	CA_OutToIdle UMETA(DisplayName = "OutToIdle"),
	CA_OutToIdleArm UMETA(DisplayName = "OutToIdleArm"),
	CA_ReloadCharge UMETA(DisplayName = "ReloadCharge"),
	Default_MAX UMETA(Hidden)
};
