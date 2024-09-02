#pragma once

#include "CoreMinimal.h"

#define CHARACTER_BASE_HUMAN_RIBCAGE_SOCKET_NAME					(TEXT("Base-HumanRibcage"))
#define CHARACTER_PISTOL_HOLSTER_SOCKET_NAME						(TEXT("pistol_holsterSocket"))
#define CHARACTER_PRIMARY_WEAPON_HOLSTER_SOCKET_NAME				(TEXT("weapon_holsterSocket"))
#define CHARACTER_SECONDARY_WEAPON_HOLSTER_SOCKET_NAME(Index)		(*FString::Printf(TEXT("weapon_holster%uSocket"), Index))