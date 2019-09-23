#include "bot_commands.h"
#include "standard_includes.h"
#include "enginecallback.h"
#include "eiface.h"
#include "gamerules.h"
#include "botgamerulesinterface.h"

namespace BotCommands
{
	void Initialise()
	{
		g_engfuncs.pfnAddServerCommand("bot_add", &Bot_Add);
		g_engfuncs.pfnAddServerCommand("bot_removeall", &Bot_RemoveAll);
	}

	void Bot_Add(void)
	{
		CHalfLifeMultiplay* mpGameRules = dynamic_cast<CHalfLifeMultiplay*>(g_pGameRules);

		if ( !mpGameRules )
		{
			ALERT(at_error, "Cannot add bots when not in multiplayer game.\n");
			return;
		}

		CBotGameRulesInterface* bgri = g_pGameRules ? g_pGameRules->BotGameRulesInterface() : NULL;

		if ( !bgri )
		{
			ALERT(at_error, "Game rules do not support bot commands.\n");
			return;
		}

		CBotFactory& botFactory = bgri->BotFactory();

		int numArgs = CMD_ARGC();

		if ( numArgs < 2 )
		{
			// Add a new bot with a random profile.
			botFactory.CreateBots(1);
			return;
		}

		if ( numArgs == 2 )
		{
			const char* arg = CMD_ARGV(1);

			if ( arg && (*arg < '0' || *arg > '9') )
			{
				// The first character was not a digit, so treat as a profile name.
				botFactory.TryCreateBot(arg);
			}
			else
			{
				// Try to parse as a number and add some random bots.
				int botCount = atoi(arg);

				if ( botCount > 0 )
				{
					botFactory.CreateBots(botCount);
				}
				else
				{
					ALERT(at_error, "Amount of bots to add must be a positive number.\n", arg);
				}
			}

			return;
		}

		for ( int argNum = 1; argNum < numArgs; ++argNum )
		{
			// Treat each argument as a bot profile.
			botFactory.TryCreateBot(CMD_ARGV(argNum));
		}
	}

	void Bot_RemoveAll(void)
	{
		if ( !g_pGameRules || !g_pGameRules->IsMultiplayer() )
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
}
