#include "botgamerulesinterface.h"

#include "utlvector.h"
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "enginecallback.h"
#include "gamerules.h"
#include "client.h"
#include "nodes.h"
#include "bot.h"
#include "botprofileparser.h"
#include "botprofiletable.h"

CBotGameRulesInterface::CBotGameRulesInterface(CGameRules* parent) :
	m_pParent(parent),
	m_nBotCount(0),
	m_ProfileTable()
{
	ASSERT(m_pParent);

	// This class will be instantiated on world precache, when the game rules are created.
	// Reload the bot profiles at this point.
	LoadBotProfiles();
}

void CBotGameRulesInterface::ClientDisconnect(edict_t* entity)
{
	if ( !entity || !(entity->v.flags & FL_FAKECLIENT) )
	{
		return;
	}

	if ( m_nBotCount > 0 )
	{
		--m_nBotCount;
	}
}

void CBotGameRulesInterface::ClientPutInServer(edict_t* entity)
{
	if ( !entity || !(entity->v.flags & FL_FAKECLIENT) )
	{
		return;
	}

	++m_nBotCount;

	// This step converts the entity into a bot
	// by linking up the edict to the CBaseBot class.
	CBaseBot* bot = GetClassPtrFromEdict<CBaseBot>(entity);

	// Should never happen, but to be safe:
	if ( !bot )
	{
		return;
	}

	m_pParent->PlayerThink(bot);
}

bool CBotGameRulesInterface::ClientCommand(CBasePlayer* player, const char* command)
{
	bool handled = true;

	if ( FStrEq(command, "bot_add") )
	{
		HandleBotAddCommand(player);
	}
	else if ( FStrEq(command, "bot_remove_all") )
	{
		HandleBotRemoveAllCommand(player);
	}
	else
	{
		handled = false;
	}

	return handled;
}

void CBotGameRulesInterface::Think()
{
	for ( int clientIndex = 1; clientIndex <= gpGlobals->maxClients; ++clientIndex )
	{
		CBasePlayer* player = UTIL_CBasePlayerByIndex(clientIndex);

		if ( !player )
		{
			continue;
		}

		if ( player->IsFakeClient() )
		{
			player->BotThink();
		}
		else if ( player->IsNetClient() && player->IsAlive() )
		{
			WorldGraph.MarkLocationFavorable(player->pev->origin);
		}
	}
}

bool CBotGameRulesInterface::CanExecuteCommand(CBasePlayer* player)
{
	if ( IS_DEDICATED_SERVER() )
	{
		// Client commands not allowed to be processed on a dedicated server.
		CLIENT_PRINTF(player->edict(), print_console, "Bots can only be added or removed by the server.\n");
		return false;
	}

	return true;
}

void CBotGameRulesInterface::HandleBotAddCommand(CBasePlayer* player)
{
	if ( !CanExecuteCommand(player) )
	{
		return;
	}

	int numArgs = CMD_ARGC();

	if ( numArgs < 2 )
	{
		// Add a new bot with a random profile.
		CreateBots(1);
		return;
	}

	if ( numArgs == 2 )
	{
		const char* arg = CMD_ARGV(1);

		if ( arg && (*arg < '0' || *arg > '9') )
		{
			// The first character was not a digit, so treat as a profile name.
			TryCreateBot(arg);
		}
		else
		{
			// Try to parse as a number and add some random bots.
			int botCount = atoi(arg);

			if ( botCount > 0 )
			{
				CreateBots(botCount);
			}
			else
			{
				ALERT(at_console, "Amount of bots to add must be a positive number.\n", arg);
			}
		}

		return;
	}

	for ( int argNum = 1; argNum < numArgs; ++argNum )
	{
		// Treat each argument as a bot profile.
		TryCreateBot(CMD_ARGV(argNum));
	}
}

void CBotGameRulesInterface::HandleBotRemoveAllCommand(CBasePlayer* player)
{
	if ( !CanExecuteCommand(player) )
	{
		return;
	}

	CUtlString kickCommands;

	for ( int clientIndex = 1; clientIndex <= gpGlobals->maxClients; ++clientIndex )
	{
		CBasePlayer* player = UTIL_CBasePlayerByIndex(clientIndex);

		if ( !player || !player->IsFakeClient() )
		{
			continue;
		}

		const char* name = STRING(player->pev->netname);
		kickCommands.AppendFormat("kick \"%s\"\n", name);
	}

	if ( !kickCommands.IsEmpty() )
	{
		SERVER_COMMAND(kickCommands.String());
	}
}

void CBotGameRulesInterface::CreateBots(uint32_t num)
{
	CUtlVector<CUtlString> randomProfileNameList;
	m_ProfileTable.RandomProfileNameList(randomProfileNameList, num);

	if ( randomProfileNameList.Count() > 0 )
	{
		FOR_EACH_VEC(randomProfileNameList, index)
		{
			CreateBot(m_ProfileTable.GetProfile(randomProfileNameList[index]));
		}
	}
	else
	{
		// No profiles loaded - use default bots.
		for ( uint32_t index = 0; index < num; ++index )
		{
			CreateBot();
		}
	}
}

void CBotGameRulesInterface::TryCreateBot(const CUtlString& profileName)
{
	if ( !m_ProfileTable.ProfileExists(profileName) )
	{
		ALERT(at_console, "Bot profile '%s' does not exist.\n", profileName.String());
		return;
	}

	CreateBot(m_ProfileTable.GetProfile(profileName));
}

void CBotGameRulesInterface::CreateBot(const CBotProfileTable::ProfileData* profile)
{
	CUtlString name("Bot");

	if ( profile )
	{
		name = profile->playerName;
	}

	name = UTIL_SanitisePlayerNetName(name);
	edict_t* bot = g_engfuncs.pfnCreateFakeClient(name.String());

	if ( !bot )
	{
		static const char* errorMsg = "No free player slots to create new bot.\n";

		UTIL_ClientPrintAll(HUD_PRINTNOTIFY, UTIL_VarArgs(errorMsg));

		if ( IS_DEDICATED_SERVER() )
		{
			printf(errorMsg);
		}

		return;
	}

	// Run the bot through the standard connection functions.
	// It'll eventually get passed back through this class.
	::ClientPutInServer(bot);

	// Now that the bot is registered with us, set up profile-specific things.
	SetBotAttributesViaProfile(dynamic_cast<CBaseBot*>(CBaseEntity::Instance(&bot->v)), profile);
	ALERT(at_console, "Added bot '%s'\n", STRING(bot->v.netname));
}

void CBotGameRulesInterface::LoadBotProfiles()
{
	const char* fileName = CVAR_GET_STRING("bot_profile_file");

	if ( !fileName || !(*fileName) )
	{
		// No bot profiles.
		return;
	}

	CUtlString filePath;
	filePath.Format("scripts/%s", fileName);

	CBotProfileParser parser(m_ProfileTable);

	if ( !parser.Parse(filePath) )
	{
		ALERT(at_error, "Could not load bot profiles from '%s'\n", filePath.String());
	}
}

void CBotGameRulesInterface::SetBotAttributesViaProfile(CBaseBot* bot, const CBotProfileTable::ProfileData* profile)
{
	if ( !bot || !profile )
	{
		return;
	}

	SetBotSkin(bot, profile->skin);
}

void CBotGameRulesInterface::SetBotSkin(CBaseBot* bot, const CUtlString& skin)
{
	if ( !bot )
	{
		return;
	}

	// Annoyingly this takes a non-const char* (can we change this one day?),
	// but the function just uses it as const internally so it's OK to cast.
	g_engfuncs.pfnSetClientKeyValue(bot->entindex(),
									g_engfuncs.pfnGetInfoKeyBuffer(bot->edict()),
									"model",
									(char*)CUtlString(skin).String());
}