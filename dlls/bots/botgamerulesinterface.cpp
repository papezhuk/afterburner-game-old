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
	m_nNextBotNumber(0)
{
	ASSERT(m_pParent);
}

void CBotGameRulesInterface::ClientDisconnect(edict_t* pEntity)
{
}

void CBotGameRulesInterface::ClientPutInServer(edict_t* pEntity)
{
	if ( !pEntity || !(pEntity->v.flags & FL_FAKECLIENT) )
	{
		return;
	}

	// This step converts the entity into a bot
	// by linking up the edict to the CBaseBot class.
	CBaseBot* bot = GetClassPtrFromEdict<CBaseBot>(pEntity);

	// Should never happen, but to be safe:
	if ( !bot )
	{
		return;
	}

	m_pParent->PlayerThink(bot);
}

bool CBotGameRulesInterface::ClientCommand(CBasePlayer* pPlayer, const char* pcmd)
{
	bool handled = true;

	if ( FStrEq(pcmd, "bot_add") )
	{
		CreateBot();
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

void CBotGameRulesInterface::CreateBot()
{
	std::string name = std::string("Bot") + std::to_string(m_nNextBotNumber++);
	edict_t* bot = g_engfuncs.pfnCreateFakeClient(name.c_str());

	if ( !bot )
	{
		static const char* errorMsg = "There are no free client slots. Set maxplayers to a higher value.\n";

		UTIL_ClientPrintAll(HUD_PRINTNOTIFY, UTIL_VarArgs(errorMsg));

		if (IS_DEDICATED_SERVER())
		{
			printf(errorMsg);
		}

		return;
	}

	// Run the bot through the standard connection functions.
	// It'll eventually get passed back through this class.
	::ClientPutInServer(bot);
}