#include "bot_commands.h"
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "nodes.h"
#include "bot_misc.h"
#include "bot_cvars.h"

namespace Bot_Commands
{
	void AddBot(CBasePlayer* pPlayer, const char* pcmd)
	{
		if (!IS_DEDICATED_SERVER())
		{
			int NumPlayersInGameNow = 0;

			for ( int i = 1; i <= gpGlobals->maxClients; i++ )
			{
				CBasePlayer* player = (CBasePlayer*)UTIL_PlayerByIndex(i);

				if ( player && player->SpawnIndex > -1)
				{
					NumPlayersInGameNow++;
				}
			}

			if ( NumPlayersInGameNow < MAX_BOTS )
			{
				char serverCommand[128];
				snprintf(serverCommand, 127, "bot_number %d\n", ((int)(bot_number.value)+1) );
				SERVER_COMMAND(serverCommand);
			}
			else
			{
				char buffer[128];
				snprintf(buffer, 127, "Maximum number of bots is %d.\n", MAX_BOTS);
				UTIL_ClientPrintAll( HUD_PRINTNOTIFY, buffer );
			}
		}
		else
		{
			// If not dedicated server, this is the server DLL on a connected client's computer.
			CLIENT_PRINTF( pPlayer->edict(), print_console, "addbot not allowed from client!\n" );
		}
	}

	void RemoveAllBots(CBasePlayer* pPlayer, const char* pcmd)
	{
		if (!IS_DEDICATED_SERVER())
		{
			SERVER_COMMAND("bot_number 0");
		}
		else
		{
			CLIENT_PRINTF( pPlayer->edict(), print_console, "removebots not allowed from client!\n" );
		}
	}

	void RemoveBot(CBasePlayer* pPlayer, const char* pcmd)
	{
		if (!IS_DEDICATED_SERVER())
		{
			CBasePlayer *somePlayer = (CBasePlayer *)UTIL_FindEntityByString( NULL, "netname", (const char*)CMD_ARGV(1) );

			if ( somePlayer )
			{
				//Scott: Set respawn flag, prevents respawning removed bot
				bot_respawn[somePlayer->SpawnIndex].is_used = FALSE;
				somePlayer->SpawnIndex = -1;

				somePlayer->pev->health = 0;

				char serverKickCommand[128];
				snprintf( serverKickCommand, 127, "kick \"%s\"\n", STRING(somePlayer->pev->netname) );

				CLIENT_COMMAND( pPlayer->edict(), serverKickCommand );

				char serverCommand[128];
				snprintf(serverCommand, 127, "bot_number %d\n", ((int)(bot_number.value)-1) );
				SERVER_COMMAND(serverCommand);
			}
			else
			{
				char Command[128];
				snprintf( Command, 127, "could not kick \"%s\"\n", (const char*)CMD_ARGV(1) );
				CLIENT_PRINTF( pPlayer->edict(), print_console, Command );
			}
		}
		else
		{
			CLIENT_PRINTF( pPlayer->edict(), print_console, "removebot not allowed from client!\n" );
		}
	}

	void LoadNav(CBasePlayer* pPlayer, const char* cmd)
	{
		WorldGraph.FLoadGraph( (char *)STRING( gpGlobals->mapname ) );
	}

	void SaveNav(CBasePlayer* pPlayer, const char* cmd)
	{
		WorldGraph.SaveNavToFile();
	}
}