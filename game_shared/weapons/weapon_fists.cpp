#include "weapon_fists.h"
#include "weapon_pref_weights.h"
#include "weapon_fists_atts.h"

LINK_ENTITY_TO_CLASS(weapon_fists, CWeaponFists);
LINK_ENTITY_TO_CLASS(weapon_dukes, CWeaponFists);	// For NF compatibility

CWeaponFists::CWeaponFists() : CGenericWeapon()
{
}

const WeaponAtts::WACollection& CWeaponFists::WeaponAttributes() const
{
	return StaticWeaponAttributes;
}

#ifndef CLIENT_DLL
float CWeaponFists::Bot_CalcDesireToUse(CBaseBot& bot, CBaseEntity& enemy, float distanceToEnemy) const
{
	// TODO
	return 0;
}

void CWeaponFists::Bot_SetFightStyle(CBaseBotFightStyle& fightStyle) const
{
	// TODO
}
#endif
