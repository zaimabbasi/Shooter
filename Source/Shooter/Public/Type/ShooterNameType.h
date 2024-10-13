#pragma once

#include "CoreMinimal.h"

// Character Types
#define CHARACTER_BASE_HUMAN_L_COLLARBONE_SOCKET_NAME				(TEXT("Base-HumanLCollarbone"))
#define CHARACTER_BASE_HUMAN_L_PALM_SOCKET_NAME						(TEXT("Base-HumanLPalm"))
#define CHARACTER_BASE_HUMAN_R_COLLARBONE_SOCKET_NAME				(TEXT("Base-HumanRCollarbone"))
#define CHARACTER_BASE_HUMAN_R_PALM_SOCKET_NAME						(TEXT("Base-HumanRPalm"))
#define CHARACTER_BASE_HUMAN_RIBCAGE_SOCKET_NAME					(TEXT("Base-HumanRibcage"))
#define CHARACTER_CAMERA_ANIMATED_SOCKET_NAME						(TEXT("Camera_animated"))
#define CHARACTER_CAMERA_ANIMATED_3RD_SOCKET_NAME					(TEXT("Camera_animated_3rd"))
#define CHARACTER_IK_S_L_PALM_SOCKET_NAME							(TEXT("IK_S_LPalm"))
#define CHARACTER_IK_S_R_PALM_SOCKET_NAME							(TEXT("IK_S_RPalm"))
#define CHARACTER_PISTOL_HOLSTER_SOCKET_NAME						(TEXT("pistol_holsterSocket"))
#define CHARACTER_PRIMARY_WEAPON_HOLSTER_SOCKET_NAME				(TEXT("weapon_holsterSocket"))
#define CHARACTER_SECONDARY_WEAPON_HOLSTER_SOCKET_NAME(Index)		(*FString::Printf(TEXT("weapon_holster%uSocket"), Index))


// Mod Types
#define MOD_BARREL_SOCKET_NAME										(TEXT("mod_barrel"))
#define MOD_CHARGE_SOCKET_NAME										(TEXT("mod_charge"))
#define MOD_GAS_BLOCK_SOCKET_NAME									(TEXT("mod_gas_block"))
#define MOD_HANDGUARD_SOCKET_NAME									(TEXT("mod_handguard"))
#define MOD_MAGAZINE_SOCKET_NAME									(TEXT("mod_magazine"))
#define MOD_MUZZLE_SOCKET_NAME										(TEXT("mod_muzzle"))
#define MOD_PISTOL_GRIP_SOCKET_NAME									(TEXT("mod_pistol_grip"))
#define MOD_RECIEVER_SOCKET_NAME									(TEXT("mod_reciever"))
#define MOD_SIGHT_FRONT_SOCKET_NAME									(TEXT("mod_sight_front"))
#define MOD_SIGHT_REAR_SOCKET_NAME									(TEXT("mod_sight_rear"))
#define MOD_STOCK_SOCKET_NAME										(TEXT("mod_stock"))
#define PATRON_SOCKET_NAME(Index)									(*FString::Printf(TEXT("patron_%03d"), Index))

// Weapon Types
#define WEAPON_PATRON_IN_WEAPON_SOCKET_NAME							(TEXT("patron_in_weapon"))
#define WEAPON_SHELL_PORT_SOCKET_NAME								(TEXT("shellport"))
