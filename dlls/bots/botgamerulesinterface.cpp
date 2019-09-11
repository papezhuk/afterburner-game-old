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
#include "bot_misc.h"

CBotGameRulesInterface::CBotGameRulesInterface(CGameRules* parent) :
	m_pParent(parent),
	m_BotFactory()
{
	ASSERT(m_pParent);

	// This class will be instantiated on world precache, when the game rules are created.
	// Reload the bot profiles at this point.
	m_BotFactory.LoadBotProfiles();
}

void CBotGameRulesInterface::ClientDisconnect(edict_t* entity)
{

}

void CBotGameRulesInterface::ClientPutInServer(edict_t* entity)
{
	if ( !entity || !(entity->v.flags & FL_FAKECLIENT) )
	{
		return;
	}

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
	return false;
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

	BotGlobals.HandleAutoAdjDifficulty();
}

CBotFactory& CBotGameRulesInterface::BotFactory()
{
	return m_BotFactory;
}

const CBotFactory& CBotGameRulesInterface::BotFactory() const
{
	return m_BotFactory;
}
