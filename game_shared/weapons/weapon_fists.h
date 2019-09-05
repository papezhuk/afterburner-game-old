#pragma once

#include "standard_includes.h"
#include "genericmeleeweapon.h"

class CWeaponFists : public CGenericMeleeWeapon
{
public:
	CWeaponFists();
	virtual const WeaponAtts::WACollection& WeaponAttributes() const override;

#ifndef CLIENT_DLL
	virtual float Bot_CalcDesireToUse(CBaseBot& bot, CBaseEntity& enemy, float distanceToEnemy) const override;
	virtual void Bot_SetFightStyle(CBaseBotFightStyle& fightStyle) const override;
#endif
};

namespace WeaponAtts
{
	template<>
	const struct WACollection& StaticWeaponAttributes<CWeaponFists>();
}
