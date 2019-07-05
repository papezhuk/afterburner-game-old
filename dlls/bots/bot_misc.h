
/**************************************************
 * Rho-Bot Source Code                            *
 * by Eric Bieschke (rhobot@botepidemic.com)      *
 * http://www.botepidemic.com/rhobot/             *
 **************************************************
 * You are free to do whatever you want with this *
 * source. All I ask is that (A) you tell me what *
 * you do with it so that I can check it out, (B) *
 * you credit me for any of my code that you use  *
 * in your project, and (C) if you are now, or    *
 * later become involved in the gaming industry   *
 * and think I would be a valuable contribution   *
 * to the team, contact me.   =]                  *
 **************************************************
 * If you have questions about the source, please *
 * don't hesitate to give me a ring. If you make  *
 * improvements to this source that you'd like to *
 * share with others, please let me know and I'll *
 * release your changes with the next source code *
 * release (you will be fully credited for all of *
 * your work). If I inadvertantly did not credit  *
 * either botman or Rich Whitehouse for code taken*
 * from their respective open-source bots, I      *
 * apologize (I did my best to comment what was   *
 * theirs), and if you let me know, I will credit *
 * them in the next source release.               *
 **************************************************/

#ifndef RHOBOT_MISC_H
#define RHOBOT_MISC_H

//#include "nodes.h"

typedef struct
{
   BOOL is_used;
   char name[32];
} respawn_t;

///////////////////////////////////////////////////////////////////////////////
// Global Vars
///////////////////////////////////////////////////////////////////////////////

const int MAX_BOTS			= 32;
const int MAX_BOT_SKINS		= 16;
const int MAX_BOT_NAMES		= 37;
const int MAX_LINE_LENGTH	= 100;

///////////////////////////////////////////////////////////////////////////////
// Global Funcs
///////////////////////////////////////////////////////////////////////////////

void nextItem(char *dest, char **line, char delimiter, char comment = '\0');
//void BotConnect( edict_t *aEntity);
void BotConnect( int NumBots);
void BotClientPutInServer( edict_t *pEntity, char *pickedBot );

///////////////////////////////////////////////////////////////////////////////
// CBaseBotGlobals Class
///////////////////////////////////////////////////////////////////////////////

class CBaseBotGlobals
{
public:

	float	AutoAdjCheckTime;
//	int	AutoAdjustDifficulty; //Starbreaker - Change from BOOL to int
//	float	ChatPropensity;
//	float	DifficultyLevel;
//	float	JumpPropensity;
	char	LastUsedBotName[MAX_LINE_LENGTH];
//	int		NumBotsDefault;

	CBaseBotGlobals();
	~CBaseBotGlobals();

	void	IncreaseDifficulty( void );
	void	DecreaseDifficulty( void );
	void	HandleAutoAdjDifficulty( void );
//	BOOL	ReadRhobotCfg( void );

};

extern CBaseBotGlobals BotGlobals;
extern BOOL skin_used[MAX_BOT_SKINS];
//Scott: Style change.  Did not like pointer reference.
//       changed "char *bot_skins[MAX_BOT_SKINS]" to "bot_skins[MAX_BOT_SKINS][64]".
extern char bot_skins[MAX_BOT_SKINS][64];
extern BOOL name_used[MAX_BOT_NAMES];
//Scott: Style change.  Did not like pointer reference.
//       changed "char *bot_names[MAX_BOT_NAMES]" to "bot_names[MAX_BOT_NAMES]".
extern char bot_names[MAX_BOT_NAMES][64];
extern respawn_t bot_respawn[32];

BOOL UseBotCycle(int NumBots);

#endif	/*RHOBOT_MISC_H*/

