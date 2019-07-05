#ifndef BOT_COMMANDS_H
#define BOT_COMMANDS_H

class CBasePlayer;

namespace Bot_Commands
{
	void AddBot(CBasePlayer* pPlayer, const char* pcmd);
	void RemoveAllBots(CBasePlayer* pPlayer, const char* pcmd);
	void RemoveBot(CBasePlayer* pPlayer, const char* cmd);
	void LoadNav(CBasePlayer* pPlayer, const char* cmd);
	void SaveNav(CBasePlayer* pPlayer, const char* cmd);
}

#endif // BOT_COMMANDS_H