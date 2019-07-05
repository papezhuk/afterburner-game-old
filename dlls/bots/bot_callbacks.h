#ifndef BOT_CALLBACKS_H
#define BOT_CALLBACKS_H

class CBasePlayer;
typedef struct edict_s edict_t;

// For now, this is probably the best way to do things,
// without massively over-complicating everything by essentially
// re-programming my own version of Metamod.
// Just localise all of the bot-specific code to this pair
// of files so that it's not cluttering the already very
// cluttered HL code.

namespace Bot_Callbacks
{
	void ClientDisconnect(edict_t* pEntity);
	void ClientPutInServer(edict_t* pEntity);
	bool ClientCommand(CBasePlayer* pPlayer, const char* pcmd);
	void PlayerPreThink(edict_t* pEntity);
	void StartFrame(void);
}

#endif // BOT_CALLBACKS_H