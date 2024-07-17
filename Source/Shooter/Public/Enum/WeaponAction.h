#pragma once

#include "CoreMinimal.h"
#include "WeaponAction.generated.h"

UENUM(BlueprintType)
enum class EWeaponAction : uint8
{
	WAS_Idle UMETA(DisplayName = "Idle"),
	WAS_IdleToOut UMETA(DisplayName = "IdleToOut"),
	WAS_OutToIdle UMETA(DisplayName = "OutToIdle"),
	Default_MAX UMETA(Hidden)
};