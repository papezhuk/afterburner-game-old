#include "botgamerulesinterface.h"

#include <string>
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

	std::string kickCommands;

	for ( int clientIndex = 1; clientIndex <= gpGlobals->maxClients; ++clientIndex )
	{
		CBasePlayer* player = UTIL_CBasePlayerByIndex(clientIndex);

		if ( !player || !player->IsFakeClient() )
		{
			continue;
		}

		const char* name = STRING(player->pev->netname);
		kickCommands += std::string("kick \"") + std::string(name) + std::string("\"\n");
	}

	if ( kickCommands.size() > 0 )
	{
		SERVER_COMMAND(kickCommands.c_str());
	}
}

void CBotGameRulesInterface::CreateBots(uint32_t num)
{
	for ( uint32_t iteration = 0; iteration < num; ++iteration )
	{
		// TODO: Choose a random profile
		CreateBot();
	}
}

void CBotGameRulesInterface::TryCreateBot(const std::string& profileName)
{
	if ( !m_ProfileTable.ProfileExists(profileName) )
	{
		ALERT(at_console, "Bot profile '%s' does not exist.\n", profileName.c_str());
		return;
	}

	CreateBot(m_ProfileTable.GetProfile(profileName));
}

void CBotGameRulesInterface::CreateBot(const CBotProfileTable::ProfileData* profile)
{
	std::string name("Bot");

	if ( profile )
	{
		name = profile->playerName;
	}

	// TODO: Validate name!

	edict_t* bot = g_engfuncs.pfnCreateFakeClient(name.c_str());

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

	std::string filePath = std::string("scripts/") + std::string(fileName);

	CBotProfileParser parser(m_ProfileTable);

	if ( !parser.Parse(filePath) )
	{
		ALERT(at_error, "Could not load bot profiles from '%s'\n", filePath.c_str());
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

void CBotGameRulesInterface::SetBotSkin(CBaseBot* bot, const std::string& skin)
{
	if ( !bot )
	{
		return;
	}

	// The call takes a non-const char*, so we do this just in case it mucks about with it.
	char buffer[64];
	buffer[0] = '\0';

	if ( skin.size() < 64 )
	{
		strncpy(buffer, skin.c_str(), sizeof(buffer));
		buffer[sizeof(buffer) - 1] = '\0';
	}
	else
	{
		ALERT(at_console, "Skin name '%s' for bot '%s' was too long!\n", skin.c_str(), STRING(bot->pev->netname));
	}

	g_engfuncs.pfnSetClientKeyValue(bot->entindex(),
									g_engfuncs.pfnGetInfoKeyBuffer(bot->edict()),
									"model",
									buffer);
}