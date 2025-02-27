#pragma once
#include "CoreMinimal.h"
#include "WeaponTypes.generated.h"

UENUM(BlueprintType)
enum class EWeaponFiremode : uint8
{
	WF_None UMETA(DisplayName = "None"),
	WF_SingleShot UMETA(DisplayName = "SingleShot"),
	WF_2RoundsBurst UMETA(DisplayName = "2RoundsBurst"),
	WF_3RoundsBurst UMETA(DisplayName = "3RoundsBurst"),
	WF_FullAuto UMETA(DisplayName = "FullAuto"),
	Default_MAX UMETA(Hidden)
};
