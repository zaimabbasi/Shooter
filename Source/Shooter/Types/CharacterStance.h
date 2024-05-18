#pragma once

#include "CoreMinimal.h"
#include "CharacterStance.generated.h"

UENUM(BlueprintType)
enum class ECharacterStance : uint8
{
	CS_Stand UMETA(DisplayName = "Stand"),
	CS_Crouch UMETA(DisplayName = "Crouch"),
	CS_Prone UMETA(DisplayName = "Prone"),
	Default_MAX UMETA(Hidden)
};