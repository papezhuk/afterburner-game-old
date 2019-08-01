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

CBotGameRulesInterface::CBotGameRulesInterface(CGameRules* parent) :
	m_pParent(parent),
	m_nNextBotNumber(0),
	m_nBotCount(0)
{
	ASSERT(m_pParent);
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

		if ( player->IsBot() )
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
		const char* botCountStr = CMD_ARGV(1);
		int botCount = atoi(botCountStr);

		if ( botCount > 0 )
		{
			// Add a specified number of bots.
			CreateBots(botCount);
		}
		else if ( botCount == 0 )
		{
			// See whether the argument matches a bot profile.
			// TODO
		}

		return;
	}

	for ( int argNum = 1; argNum < numArgs; ++argNum )
	{
		// Treat each argument as a bot profile.
		const char* arg = CMD_ARGV(argNum);

		// TODO
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

		if ( !player || !player->IsBot() )
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
		std::string name = std::string("Bot") + std::to_string(m_nNextBotNumber++);
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
	}
}