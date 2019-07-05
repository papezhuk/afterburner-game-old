/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#pragma once
#ifndef GAME_H
#define GAME_H

extern void GameDLLInit( void );

extern cvar_t displaysoundlist;

// multiplayer server rules
extern cvar_t teamplay;
extern cvar_t fraglimit;
extern cvar_t timelimit;
extern cvar_t friendlyfire;
extern cvar_t falldamage;
extern cvar_t weaponstay;
extern cvar_t selfgauss;
extern cvar_t satchelfix;
extern cvar_t forcerespawn;
extern cvar_t flashlight;
extern cvar_t aimcrosshair;
extern cvar_t decalfrequency;
extern cvar_t teamlist;
extern cvar_t teamoverride;
extern cvar_t defaultteam;
extern cvar_t allowmonsters;

// Engine Cvars
extern cvar_t *g_psv_gravity;
extern cvar_t *g_psv_aim;
extern cvar_t *g_footsteps;

//BEGIN RHO-BOT:
extern cvar_t bot_blood;
extern cvar_t bot_speed;
extern cvar_t bot_number;
extern cvar_t bot_skill_aa;
extern cvar_t bot_skill;
extern cvar_t bot_jump;
extern cvar_t bot_chat;

//cvars for debugging
extern cvar_t bot_debug; // activate debug options
extern cvar_t bot_dontshoot; // does not shoot at humans or bots
extern cvar_t bot_observer; // does not shoot at humans

// bot spawning cvars
extern cvar_t bot_name;
extern cvar_t bot_model;
extern cvar_t bot_top_color;
extern cvar_t bot_bottom_color;
extern cvar_t bot_trait_jump;
extern cvar_t bot_trait_accuracy;
extern cvar_t bot_trait_aggression;
extern cvar_t bot_trait_chat;
extern cvar_t bot_trait_perception;
extern cvar_t bot_trait_reflexes;
//END RHO-BOT

#endif // GAME_H
