#pragma once

#include <cstdint>
#include "mathlib.h"
#include "const.h"
#include "botprofiletable.h"
#include "utlstring.h"

class CBasePlayer;
class CBaseBot;
class CGameRules;

class CBotGameRulesInterface
{
public:
	CBotGameRulesInterface(CGameRules* parent);

	void ClientDisconnect(edict_t* entity);
	void ClientPutInServer(edict_t* entity);
	bool ClientCommand(CBasePlayer* player, const char* command);
	void Think();

private:
	void LoadBotProfiles();
	void CreateBots(uint32_t num);
	void CreateBot(const CBotProfileTable::ProfileData* profile = NULL);
	void TryCreateBot(const CUtlString& profileName);

	void SetBotAttributesViaProfile(CBaseBot* bot, const CBotProfileTable::ProfileData* profile);
	void SetBotSkin(CBaseBot* bot, const CUtlString& skin);

	bool CanExecuteCommand(CBasePlayer* player);
	void HandleBotAddCommand(CBasePlayer* player);
	void HandleBotRemoveAllCommand(CBasePlayer* player);

	CGameRules* m_pParent;
	uint32_t m_nBotCount;
	CBotProfileTable m_ProfileTable;
};