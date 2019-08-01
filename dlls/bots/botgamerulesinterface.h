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

	void ClientDisconnect(edict_t* entity);
	void ClientPutInServer(edict_t* entity);
	bool ClientCommand(CBasePlayer* player, const char* command);
	void Think();

	void CreateBots(uint32_t num);

private:
	bool CanExecuteCommand(CBasePlayer* player);
	void HandleBotAddCommand(CBasePlayer* player);
	void HandleBotRemoveAllCommand(CBasePlayer* player);

	CGameRules* m_pParent;
	uint32_t m_nNextBotNumber;
	uint32_t m_nBotCount;
};

#endif // BOTGAMERULESINTERFACE_H