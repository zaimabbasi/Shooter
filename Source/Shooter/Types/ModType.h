#pragma once

#include "CoreMinimal.h"
#include "ModType.generated.h"

UENUM(BlueprintType)
enum class EModType : uint8
{
	MT_Barrel UMETA(DisplayName = "Barrel"),
	MT_Charge UMETA(DisplayName = "Charge"),
	MT_GasBlock UMETA(DisplayName = "GasBlock"),
	MT_Handguard UMETA(DisplayName = "Handguard"),
	MT_Mag UMETA(DisplayName = "Mag"),
	MT_Muzzle UMETA(DisplayName = "Muzzle"),
	MT_Pistolgrip UMETA(DisplayName = "Pistolgrip"),
	MT_Reciever UMETA(DisplayName = "Reciever"),
	MT_SightFront UMETA(DisplayName = "SightFront"),
	MT_SightRear UMETA(DisplayName = "SightRear"),
	MT_Stock UMETA(DisplayName = "Stock"),
	Default_MAX UMETA(Hidden)
};