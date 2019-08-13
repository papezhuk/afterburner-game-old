#include "bot_cvars.h"
#include "extdll.h"
#include "enginecallback.h"

cvar_t bot_speed = {"bot_speed","300" };
cvar_t bot_number = { "bot_number" , "3" };
cvar_t bot_skill = { "bot_skill" , "50" };
cvar_t bot_skill_aa = { "bot_skill_aa" , "1" };
cvar_t bot_jump = { "bot_jump" , "50" };
cvar_t bot_chat = { "bot_chat" , "30" };
cvar_t bot_dontshoot = { "bot_dontshoot" , "0" };
cvar_t bot_observer = { "bot_observer" , "0" };
cvar_t bot_name = { "bot_name" , "Rho-Bot" };
cvar_t bot_model = { "bot_model" , "robo" };
cvar_t bot_top_color = { "bot_top_color" , "255" };
cvar_t bot_bottom_color	= { "bot_bottom_color" , "255" };
cvar_t bot_trait_jump = { "bot_trait_jump" , "50" };
cvar_t bot_trait_accuracy = { "bot_trait_accuracy" , "50" };
cvar_t bot_trait_aggression = { "bot_trait_aggression" , "50" };
cvar_t bot_trait_chat = { "bot_trait_chat" , "50" };
cvar_t bot_trait_perception = { "bot_trait_perception" , "50" };
cvar_t bot_trait_reflexes = { "bot_trait_reflexes" , "50" };
cvar_t bot_profile_file = { "bot_profile_file", "bot_profiles.json" };

void Bot_RegisterCVars(void)
{
	// Shame there isn't a more iterative way to do this, but I think
	// the cvars are too embedded in the existing code to go changing
	// them up for something that trivial.

	CVAR_REGISTER ( &bot_speed );
	CVAR_REGISTER ( &bot_number );
	CVAR_REGISTER ( &bot_skill );
	CVAR_REGISTER ( &bot_skill_aa );
	CVAR_REGISTER ( &bot_jump );
	CVAR_REGISTER ( &bot_chat );
	CVAR_REGISTER ( &bot_dontshoot );
	CVAR_REGISTER ( &bot_observer );
	CVAR_REGISTER ( &bot_name );
	CVAR_REGISTER ( &bot_model );
	CVAR_REGISTER ( &bot_top_color );
	CVAR_REGISTER ( &bot_bottom_color );
	CVAR_REGISTER ( &bot_trait_jump );
	CVAR_REGISTER ( &bot_trait_accuracy );
	CVAR_REGISTER ( &bot_trait_aggression );
	CVAR_REGISTER ( &bot_trait_chat );
	CVAR_REGISTER ( &bot_trait_perception );
	CVAR_REGISTER ( &bot_trait_reflexes );
	CVAR_REGISTER ( &bot_profile_file );
}