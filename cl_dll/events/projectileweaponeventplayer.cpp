#include "projectileweaponeventplayer.h"
#include "eventscripts.h"

void ProjectileWeaponEventPlayer::EventStart()
{
	if( EV_IsLocal(m_iEntIndex) )
	{
		EV_MuzzleFlash();
		AnimateViewModel();
	}

	PlayFireSound();
}