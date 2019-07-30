#ifndef BOTGAMERULESINTERFACE_H
#define BOTGAMERULESINTERFACE_H

#include <cstdint>
#include "mathlib.h"
#include "const.h"

class CBasePlayer;
class CGameRules;

class CBotGameRulesInterface
{
public:
	CBotGameRulesInterface(CGameRules* parent);

	void ClientDisconnect(edict_t* pEntity);
	void ClientPutInServer(edict_t* pEntity);
	bool ClientCommand(CBasePlayer* pPlayer, const char* pcmd);
	void Think();

	void CreateBot();

private:
	CGameRules* m_pParent;
	uint32_t m_nNextBotNumber;
};

#endif // BOTGAMERULESINTERFACE_H