#pragma once
#include "CoreMinimal.h"
#include "CombatTypes.generated.h"

UENUM(BlueprintType)
enum class ECombatAction :uint8
{
	CA_None UMETA(DisplayName = "None"),
	CA_ActionEnd UMETA(DisplayName = "ActionEnd"),
	CA_ActionStart UMETA(DisplayName = "ActionStart"),
	CA_ChamberCheck UMETA(DisplayName = "ChamberCheck"),
	CA_HandsInHalf1 UMETA(DisplayName = "HandsInHalf1"),
	CA_HandsInHalf2 UMETA(DisplayName = "HandsInHalf2"),
	CA_HandsOutHalf1 UMETA(DisplayName = "HandsOutHalf1"),
	CA_HandsOutHalf2 UMETA(DisplayName = "HandsOutHalf2"),
	CA_Fire UMETA(DisplayName = "Fire"),
	CA_FireDry UMETA(DisplayName = "FireDry"),
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
	CA_ReloadCatch UMETA(DisplayName = "ReloadCatch"),
	CA_ReloadCharge UMETA(DisplayName = "ReloadCharge"),
	Default_MAX UMETA(Hidden)
};
