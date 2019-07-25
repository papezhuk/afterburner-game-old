#include "bot_callbacks.h"
#include "bot_commands.h"
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "bot_cvars.h"
#include "bot.h"
#include "nodes.h"
#include "bot_misc.h"

// These commands were commented out in Rho-Bot.
// Leaving them excluded for now, just because I
// don't know the reasoning of why they were removed.
//#define NAV_LOAD_SAVE_COMMANDS

namespace Bot_Callbacks
{
	static bool bInitBotCycle = false;
	static float TimeNextPossibleBotConnect = 0.0f;

	void ClientDisconnect(edict_t *pEntity)
	{
		entvars_t* pev = &pEntity->v;
		CBasePlayer* pPlayer = GetClassPtr((CBasePlayer*)pev);

		pev->effects |= EF_NODRAW; // for observer fix
	}

	void ClientPutInServer(edict_t* pEntity)
	{
		entvars_t* pev = &pEntity->v;
		CBasePlayer* pPlayer = GetClassPtr( (CBasePlayer *)pev );
	}

	bool ClientCommand(CBasePlayer* pPlayer, const char* pcmd)
	{
		bool handled = true;

		if ( FStrEq(pcmd, "addbot") )
		{
			Bot_Commands::AddBot(pPlayer, pcmd);
		}
		else if ( FStrEq(pcmd, "removebots") )
		{
			Bot_Commands::RemoveAllBots(pPlayer, pcmd);
		}
		else if ( FStrEq(pcmd, "removebot") )
		{
			Bot_Commands::RemoveBot(pPlayer, pcmd);
		}
#ifdef NAV_LOAD_SAVE_COMMANDS
		else if ( FStrEq(pcmd, "loadnav") )
		{
			Bot_Commands::LoadNav(pPlayer, pcmd);
		}
		else if ( FStrEq(pcmd, "savenav") )
		{
			Bot_Commands:SaveNav(pPlayer, pcmd);
		}
#endif // NAV_LOAD_SAVE_COMMANDS
		else
		{
			handled = false;
		}

		return handled;
	}

	void PlayerPreThink(edict_t* pEntity)
	{
		// TODO: I don't like this method because the HUD text keeps fading out,
		// but until I can be arsed to write a better method, it can stay.

		CBasePlayer* pPlayer = (CBasePlayer*)GET_PRIVATE( pEntity );
		if ( !pPlayer )
		{
			return;
		}

		if (bot_debug.value != 0)
		{
			//Based on MasterStroke tutorial http://hlpp.valveworld.com/tuts/pointat.htm

			TraceResult tr;

			Vector anglesAim = pPlayer->pev->v_angle + pPlayer->pev->punchangle;
			UTIL_MakeVectors( anglesAim );
			Vector vecSrc = pPlayer->GetGunPosition( ) - gpGlobals->v_up * 2;
			Vector vecDir = gpGlobals->v_forward;

			UTIL_TraceLine(vecSrc, vecSrc + vecDir * 8192, dont_ignore_monsters, pPlayer->edict(), &tr);

			CBaseEntity *point = CBaseEntity::Instance(tr.pHit);

			if ( FClassnameIs( tr.pHit, "player" ) )
			{
				char PrintOutText[201];
				hudtextparms_t hText;

				entvars_t* pev = &tr.pHit->v;
				CBasePlayer *pPlayerSighted = GetClassPtr((CBasePlayer *)pev);

				if (pPlayerSighted->IsBot())
				{
					CBaseBot *pBot = GetClassPtr((CBaseBot *)pev);
					char Enemy[201];

					if (pBot->GetEnemy() != NULL)
					{
						sprintf (Enemy, STRING(pBot->GetEnemy()->pev->netname) );
					}
					else
						sprintf (Enemy, "NONE");

					sprintf(PrintOutText, "%s\n%s\n",
						STRING(pPlayerSighted->pev->netname), Enemy);
				}
				else
				{
					sprintf(PrintOutText, "%s\n", STRING(tr.pHit->v.netname));
				}

				memset(&hText, 0, sizeof(hText));
				hText.channel = 1;
				hText.x = 0.45;
				hText.y = 0.55;

				hText.effect = 0;

				hText.r1 = hText.g1 = hText.b1 = 255;
				hText.a1 = 255;
				hText.r2 = hText.g2 = hText.b2 = 255;
				hText.a2 = 255;
				hText.fadeinTime = 0.2;
				hText.fadeoutTime = 1;
				hText.holdTime = 1.5;
				hText.fxTime = 0.5;

				UTIL_HudMessage(pPlayer, hText, PrintOutText);
			}
		}
	}

	void StartFrame(void)
	{
		char *cmd, *arg1, *arg2, *arg3;
		static char cmd_line[80];
		static char server_cmd[80];
		static int index;
		int NumBotsMax = (int)bot_number.value;
		int NumPlayersInGameNow = 0;
		int NumBotsInGameNow = 0;

		CBasePlayer *pHumanPlayer = NULL;

		for ( int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CBasePlayer *pPlayer = UTIL_CBasePlayerByIndex( i );

			if ( pPlayer )
			{
				NumPlayersInGameNow++;

				if ( pPlayer->IsBot() )
				{
					NumBotsInGameNow++;

					pPlayer->BotThink(); // call BotThink for each bot every frame
				}
				else
				{
					if ( pPlayer->IsNetClient() )
					{
						pHumanPlayer = pPlayer;

						if ( pPlayer->IsAlive() )
						{
							WorldGraph.MarkLocationFavorable( pPlayer->pev->origin );
						}
					}
				}
			}
		}

		if ( NumBotsMax == 0 ||  gpGlobals->time < 10)
		{
			bInitBotCycle = FALSE;
		}

		else if ( NumBotsInGameNow == 0 && !bInitBotCycle)
		{
			bInitBotCycle = UseBotCycle(NumBotsInGameNow); //Init Bot Cycle
			bInitBotCycle = TRUE; // In case of corrupt or missing botcycle.txt
		}

		else if ( NumBotsInGameNow < NumBotsMax
			&& NumPlayersInGameNow < (gpGlobals->maxClients-1) // Scott - Always leave a spot open
			&& NumBotsInGameNow < MAX_BOTS
			&& ((NumBotsInGameNow == 0 && gpGlobals->time >= 10) ||
				(NumBotsInGameNow != 0 && gpGlobals->time >= TimeNextPossibleBotConnect))
			)
		{
			//Scott:  Connect the bot directly, do not issue Client Command.
			//This is needed for dedicated server.

			float BOTCONNECT_DELAY = 1;

			BotConnect(NumBotsInGameNow);
			TimeNextPossibleBotConnect = gpGlobals->time + BOTCONNECT_DELAY;
		}

		//Scott:  Remove a bot if game is full or number of bots has been changed

		if ( (NumBotsInGameNow > 0 && NumPlayersInGameNow == gpGlobals->maxClients) || NumBotsInGameNow > NumBotsMax )
		{
			for ( int i = gpGlobals->maxClients; i >= 1; i-- )
			{
				CBasePlayer *pPlayer = (CBasePlayer *)UTIL_PlayerByIndex( i );

				if ( pPlayer && pPlayer->IsBot() )
				{
					pPlayer->pev->health = 0;

					sprintf(server_cmd, "kick \"%s\"\n", STRING( pPlayer->pev->netname ) );

					SERVER_COMMAND(server_cmd);

					NumBotsInGameNow--;
					bInitBotCycle = UseBotCycle(NumBotsInGameNow); //Init Bot Cycle

					break;
				}
			}
		}

		BotGlobals.HandleAutoAdjDifficulty();
	}
}