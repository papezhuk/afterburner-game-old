#ifndef BOT_CVARS_H
#define BOT_CVARS_H

#include "cvardef.h"

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

extern cvar_t bot_profile_file;

void Bot_RegisterCVars(void);

#endif // BOT_CVARS_H