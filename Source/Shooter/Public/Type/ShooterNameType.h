#pragma once

#include "CoreMinimal.h"

// Character Types
#define CHARACTER_BASE_HUMAN_RIBCAGE_SOCKET_NAME					(TEXT("Base-HumanRibcage"))
#define CHARACTER_PISTOL_HOLSTER_SOCKET_NAME						(TEXT("pistol_holsterSocket"))
#define CHARACTER_PRIMARY_WEAPON_HOLSTER_SOCKET_NAME				(TEXT("weapon_holsterSocket"))
#define CHARACTER_SECONDARY_WEAPON_HOLSTER_SOCKET_NAME(Index)		(*FString::Printf(TEXT("weapon_holster%uSocket"), Index))

// Mod Types
#define MOD_BARREL_SOCKET_NAME										(TEXT("mod_barrelSocket"))
#define MOD_CHARGE_SOCKET_NAME										(TEXT("mod_chargeSocket"))
#define MOD_GAS_BLOCK_SOCKET_NAME									(TEXT("mod_gas_blockSocket"))
#define MOD_HANDGUARD_SOCKET_NAME									(TEXT("mod_handguardSocket"))
#define MOD_MAGAZINE_SOCKET_NAME									(TEXT("mod_magazineSocket"))
#define MOD_MUZZLE_SOCKET_NAME										(TEXT("mod_muzzleSocket"))
#define MOD_PISTOL_GRIP_SOCKET_NAME									(TEXT("mod_pistol_gripSocket"))
#define MOD_RECIEVER_SOCKET_NAME									(TEXT("mod_recieverSocket"))
#define MOD_SIGHT_FRONT_SOCKET_NAME									(TEXT("mod_sight_frontSocket"))
#define MOD_SIGHT_REAR_SOCKET_NAME									(TEXT("mod_sight_rearSocket"))
#define MOD_STOCK_SOCKET_NAME										(TEXT("mod_stockSocket"))
#define PATRON_SOCKET_NAME(Index)									(*FString::Printf(TEXT("patron_%03d"), Index))

// Weapon Types
#define WEAPON_PATRON_IN_WEAPON_SOCKET_NAME							(TEXT("patron_in_weapon"))
