#pragma once

#include "baseweaponeventplayer.h"

class ProjectileWeaponEventPlayer : public BaseWeaponEventPlayer
{
public:
	virtual ~ProjectileWeaponEventPlayer() {}

protected:
	virtual void EventStart() override;
};