
/**************************************************
 * Rho-Bot Source Code                            *
 * by Eric Bieschke (rhobot@botepidemic.com)      *
 * http://www.botepidemic.com/rhobot/             *
 **************************************************
 * You are free to do whatever you want with this *
 * source. All I ask is that (A) you tell me what *
 * you do with it so that I can check it out, (B) *
 * you credit me for any of my code that you use  *
 * in your project, and (C) if you are now, or    *
 * later become involved in the gaming industry   *
 * and think I would be a valuable contribution   *
 * to the team, contact me.   =]                  *
 **************************************************
 * If you have questions about the source, please *
 * don't hesitate to give me a ring. If you make  *
 * improvements to this source that you'd like to *
 * share with others, please let me know and I'll *
 * release your changes with the next source code *
 * release (you will be fully credited for all of *
 * your work). If I inadvertantly did not credit  *
 * either botman or Rich Whitehouse for code taken*
 * from their respective open-source bots, I      *
 * apologize (I did my best to comment what was   *
 * theirs), and if you let me know, I will credit *
 * them in the next source release.               *
 **************************************************/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "bot.h"
#include "game.h"
#include "nodes.h"

///////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor
///////////////////////////////////////////////////////////////////////////////

CBaseBotStats::CBaseBotStats():
/*
PrefCrossbow( 70 + RANDOM_FLOAT(0,20) ),
PrefCrowbar( 10 + RANDOM_FLOAT(0,20) ),
PrefEgon( 90 + RANDOM_FLOAT(0,10) ),
PrefGauss( 55 + RANDOM_FLOAT(0,20) ),
PrefGlock( 0 + RANDOM_FLOAT(0,10) ),
PrefHandGrenade( 10 + RANDOM_FLOAT(0,20) ),
PrefHornetGun( 40 + RANDOM_FLOAT(0,20) ),
PrefMP5( 80 + RANDOM_FLOAT(0,20) ),
PrefPython( 5 + RANDOM_FLOAT(0,10) ),
PrefRPG( 60 + RANDOM_FLOAT(0,20) ),
PrefSatchel( 0 + RANDOM_FLOAT(0,14) ),
PrefShotgun( 70 + RANDOM_FLOAT(0,20) ),
PrefSnark( 0 + RANDOM_FLOAT(0,20) ),
PrefTripMine( 0 + RANDOM_FLOAT(0,10) ),
*/

PrefAxe( 70 + RANDOM_FLOAT(0,20) ),
PrefGrenadelauncher( 70 + RANDOM_FLOAT(0,20) ),
PrefLightning( 70 + RANDOM_FLOAT(0,20) ),
PrefNailgun( 70 + RANDOM_FLOAT(0,20) ),
PrefQuakegun( 70 + RANDOM_FLOAT(0,20) ),
PrefRocketlauncher( 70 + RANDOM_FLOAT(0,20) ),
PrefSupernailgun( 70 + RANDOM_FLOAT(0,20) ),
PrefSupershotgun( 70 + RANDOM_FLOAT(0,20) ),

TraitAccuracy( 75 + RANDOM_FLOAT(0,25) ),
TraitAggression( 10 + RANDOM_FLOAT(0,70) ),
TraitChatPropensity( 0 + RANDOM_FLOAT(0,60) ),
TraitJumpPropensity( 5 + RANDOM_FLOAT(0,94) ),
TraitPerception( 50 + RANDOM_FLOAT(0,50) ),
TraitReflexes( 30 + RANDOM_FLOAT(0,40) )
{
	strcpy( Name, "Rho-Bot" );
	strcpy( Model, "gordon" ),
	strcpy( TopColor, "255" );
	strcpy( BottomColor, "255" );
}

CBaseBotStats::~CBaseBotStats()
{
}

///////////////////////////////////////////////////////////////////////////////
// FDifficultyAdjusted - tweaks a Trait to adjust for bot_skill.value
///////////////////////////////////////////////////////////////////////////////
//Scott: simplified logic

float CBaseBotStats::FDifficultyAdjusted( float currentTrait )
{
	float tweakedDiffLevel = bot_skill.value/50. - 1.; // [-1,1]

	float adjustedTrait = currentTrait;
	if ( tweakedDiffLevel < 0 )
	{
		adjustedTrait *= ( 1 + tweakedDiffLevel );
	}
	else
	{
		adjustedTrait += ( ( 100 - adjustedTrait ) * tweakedDiffLevel );
	}

	return adjustedTrait;
}

///////////////////////////////////////////////////////////////////////////////
// GetWeaponDesire - pass DISTANCE_MAX to DistanceToEnemy to use this like DispatchWeaponDesire
///////////////////////////////////////////////////////////////////////////////

float CBaseBotStats::FindWeaponDesire( CBasePlayerItem *pWeapon, float DistanceToEnemy )
{ // this function is tweaking the bot's base weapon preferences to suit the situation

	if ( FClassnameIs( pWeapon->pev, "weapon_axe" ) )
	{
		if ( DistanceToEnemy < DISTANCE_NEAR )
		{
			return GetPrefAxe();
		}
		else
		{
			return 0;
		}
	}
	else if ( FClassnameIs( pWeapon->pev, "weapon_grenadelauncher" ) )
	{
		if ( DistanceToEnemy > DISTANCE_NEAR )
		{
			return GetPrefGrenadelauncher();
		}
		else
		{
			return 0;
		}
	}
	else if ( FClassnameIs( pWeapon->pev, "weapon_lightning" ) )
	{
		return GetPrefLightning();
	}
	else if ( FClassnameIs( pWeapon->pev, "weapon_nailgun" ) )
	{
		return GetPrefNailgun();
	}
	else if ( FClassnameIs( pWeapon->pev, "weapon_quakegun" ) )
	{
		return GetPrefQuakegun();
	}
	else if ( FClassnameIs( pWeapon->pev, "weapon_supernailgun" ) )
	{
		return GetPrefSupernailgun();
	}
	else if ( FClassnameIs( pWeapon->pev, "weapon_supershotgun" ) )
	{
		return GetPrefSupershotgun();
	}
	else if ( FClassnameIs( pWeapon->pev, "weapon_rocketlauncher" ) )
	{
		if ( DistanceToEnemy > DISTANCE_NEAR )
		{
			return GetPrefRocketlauncher();
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

///////////////////////////////////////////////////////////////////////////////
// ReadBotFile (Scott) Now use config file, get cvar values if file exist
///////////////////////////////////////////////////////////////////////////////

BOOL CBaseBotStats::ReadBotFile( char *filename )
{

	// Check to see if file exists

	std::ifstream botFile(filename);
	if (botFile.bad())  return FALSE;

	// set the stats variables from the cvars

	char name[64];
	char model[64];
	char top_color[4];
	char bottom_color[4];

	strcpy(name, CVAR_GET_STRING( "bot_name" ) );
	strcpy(model, CVAR_GET_STRING( "bot_model" ) );
	strcpy(top_color, CVAR_GET_STRING( "bot_top_color" ) );
	strcpy(bottom_color, CVAR_GET_STRING( "bot_bottom_color" ) );

	SetName( name );
	SetModel( model );
	SetTopColor( top_color );
	SetBottomColor( bottom_color );

	SetTraitJumpPropensity( CVAR_GET_FLOAT( "bot_trait_jump" ) );
	SetTraitAccuracy( CVAR_GET_FLOAT( "bot_trait_accuracy" ) );
	SetTraitAggression( CVAR_GET_FLOAT( "bot_trait_aggression" ) );
	SetTraitChatPropensity( CVAR_GET_FLOAT( "bot_trait_chat" ) );
	SetTraitPerception( CVAR_GET_FLOAT( "bot_trait_perception" ) );
	SetTraitReflexes( CVAR_GET_FLOAT( "bot_trait_reflexes" ) );
/*
	SetPrefCrossbow( CVAR_GET_FLOAT( "bot_wp_crossbow" ) );
	SetPrefCrowbar( CVAR_GET_FLOAT( "bot_wp_crowbar" ) );
	SetPrefEgon( CVAR_GET_FLOAT( "bot_wp_egon" ) );
	SetPrefGauss( CVAR_GET_FLOAT( "bot_wp_gauss" ) );
	SetPrefGlock( CVAR_GET_FLOAT( "bot_wp_glock" ) );
	SetPrefHandGrenade( CVAR_GET_FLOAT( "bot_wp_handgrenade" ) );
	SetPrefHornetGun( CVAR_GET_FLOAT( "bot_wp_hornetgun" ) );
	SetPrefMP5( CVAR_GET_FLOAT( "bot_wp_mp5" ) );
	SetPrefPython( CVAR_GET_FLOAT( "bot_wp_python" ) );
	SetPrefRPG( CVAR_GET_FLOAT( "bot_wp_rpg" ) );
	SetPrefSatchel( CVAR_GET_FLOAT( "bot_wp_satchel" ) );
	SetPrefShotgun( CVAR_GET_FLOAT( "bot_wp_shotgun" ) );
	SetPrefSnark( CVAR_GET_FLOAT( "bot_wp_snark" ) );
	SetPrefTripMine( CVAR_GET_FLOAT( "bot_wp_tripmine" ) );
*/

	// Quake weapons
	SetPrefAxe ( CVAR_GET_FLOAT( "bot_wp_axe" ) );
	SetPrefGrenadelauncher ( CVAR_GET_FLOAT( "bot_wp_grenadelauncher" ) );
	SetPrefLightning ( CVAR_GET_FLOAT( "bot_wp_lightning" ) );
	SetPrefNailgun ( CVAR_GET_FLOAT( "bot_wp_nailgun" ) );
	SetPrefQuakegun ( CVAR_GET_FLOAT( "bot_wp_quakegun" ) );
	SetPrefRocketlauncher ( CVAR_GET_FLOAT( "bot_wp_rocketlauncher" ) );
	SetPrefSupernailgun ( CVAR_GET_FLOAT( "bot_wp_supernailgun" ) );
	SetPrefSupershotgun ( CVAR_GET_FLOAT( "bot_wp_supershotgun" ) );

	return TRUE;
}


