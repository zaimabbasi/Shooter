#pragma once

#include "CoreMinimal.h"

// Character Types
#define BASE_HUMAN_L_COLLARBONE_SOCKET_NAME					(TEXT("Base-HumanLCollarbone"))
#define BASE_HUMAN_L_PALM_SOCKET_NAME						(TEXT("Base-HumanLPalm"))
#define BASE_HUMAN_R_COLLARBONE_SOCKET_NAME					(TEXT("Base-HumanRCollarbone"))
#define BASE_HUMAN_R_PALM_SOCKET_NAME						(TEXT("Base-HumanRPalm"))
#define BASE_HUMAN_RIBCAGE_SOCKET_NAME						(TEXT("Base-HumanRibcage"))
#define BASE_HUMAN_SPINE3_SOCKET_NAME						(TEXT("Base-HumanSpine3"))
#define CAMERA_ANIMATED_SOCKET_NAME							(TEXT("Camera_animated"))
#define CAMERA_ANIMATED_3RD_SOCKET_NAME						(TEXT("Camera_animated_3rd"))
#define IK_S_L_PALM_SOCKET_NAME								(TEXT("IK_S_LPalm"))
#define IK_S_R_PALM_SOCKET_NAME								(TEXT("IK_S_RPalm"))
#define L_COLLARBONE_ANIM_SOCKET_NAME						(TEXT("LCollarbone_anim"))
#define PISTOL_HOLSTER_SOCKET_NAME							(TEXT("pistol_holsterSocket"))
#define R_COLLARBONE_ANIM_SOCKET_NAME						(TEXT("RCollarbone_anim"))
#define ROOT_JOINT_SOCKET_NAME								(TEXT("Root_Joint"))
#define WEAPON_HOLSTER_SOCKET_NAME							(TEXT("weapon_holsterSocket"))
#define WEAPON_HOLSTER_N_SOCKET_NAME(Index)					(*FString::Printf(TEXT("weapon_holster%uSocket"), Index))
#define WEAPON_ROOT_3RD_ANIM_SOCKET_NAME					(TEXT("Weapon_root_3rd_anim"))
#define WEAPON_ROOT_ANIM_SOCKET_NAME						(TEXT("Weapon_root_anim"))
#define WEAPON_ROOT_SOCKET_NAME								(TEXT("Weapon_root"))

// Common Types
#define BEND_GOAL_LEFT_SOCKET_NAME							(TEXT("Bend_Goal_Left"))
#define BEND_GOAL_RIGHT_SOCKET_NAME							(TEXT("Bend_Goal_Right"))

// Mod Types
#define MOD_AIM_CAMERA_SOCKET_NAME							(TEXT("mod_aim_camera"))
#define MOD_BARREL_SOCKET_NAME								(TEXT("mod_barrel"))
#define MOD_CHARGE_SOCKET_NAME								(TEXT("mod_charge"))
#define MOD_FOREGRIP_SOCKET_NAME							(TEXT("mod_foregrip"))
#define MOD_GAS_BLOCK_SOCKET_NAME							(TEXT("mod_gas_block"))
#define MOD_HANDGUARD_SOCKET_NAME							(TEXT("mod_handguard"))
#define MOD_MAGAZINE_SOCKET_NAME							(TEXT("mod_magazine"))
#define MOD_MOUNT_003_SOCKET_NAME							(TEXT("mod_mount_003"))
#define MOD_MUZZLE_SOCKET_NAME								(TEXT("mod_muzzle"))
#define MOD_PISTOL_GRIP_SOCKET_NAME							(TEXT("mod_pistol_grip"))
#define MOD_RECEIVER_SOCKET_NAME							(TEXT("mod_reciever"))
#define MOD_SIGHT_FRONT_SOCKET_NAME							(TEXT("mod_sight_front"))
#define MOD_SIGHT_REAR_SOCKET_NAME							(TEXT("mod_sight_rear"))
#define MOD_STOCK_SOCKET_NAME								(TEXT("mod_stock"))
#define PATRON_SOCKET_NAME(Index)							(*FString::Printf(TEXT("patron_%03d"), Index))

// Weapon Types
#define AIM_CAMERA_SOCKET_NAME								(TEXT("aim_camera"))
#define WEAPON_SOCKET_NAME									(TEXT("weapon"))
#define WEAPON_L_COLLARBONE_MARKER_SOCKET_NAME				(TEXT("weapon_LCollarbone_marker"))
#define WEAPON_L_HAND_MARKER_SOCKET_NAME					(TEXT("weapon_L_hand_marker"))	// Sticks (sometimes not) with the palm in animations
#define WEAPON_L_IK_MARKER_SOCKET_NAME						(TEXT("weapon_L_IK_marker"))	// Sticks with the palm in animations
#define PATRON_IN_WEAPON_SOCKET_NAME						(TEXT("patron_in_weapon"))
#define WEAPON_R_COLLARBONE_MARKER_SOCKET_NAME				(TEXT("weapon_RCollarbone_marker"))
#define WEAPON_R_HAND_MARKER_SOCKET_NAME					(TEXT("weapon_R_hand_marker"))	// Sticks (sometimes not) with the palm in animations
#define WEAPON_R_IK_MARKER_SOCKET_NAME						(TEXT("weapon_R_IK_marker"))	// Sticks with the palm in animations
#define SHELL_PORT_SOCKET_NAME								(TEXT("shellport"))

// Weapon MetaSound Trigger Types
#define CATCH_TRIGGER_NAME									(TEXT("Catch"))
#define FIRE_TRIGGER_NAME									(TEXT("Fire"))
#define FIRE_STOP_TRIGGER_NAME								(TEXT("FireStop"))
#define HAMMER_TRIGGER_NAME									(TEXT("Hammer"))
#define TRIGGER_TRIGGER_NAME								(TEXT("Trigger"))
