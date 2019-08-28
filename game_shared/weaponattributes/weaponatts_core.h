#pragma once

#include <cstdint>
#include "genericweaponattributedefs.h"
#include "weaponids.h"
#include "util.h"

class CBasePlayerWeapon;
struct CAmmoDef;

class CGenericWeaponAtts_Core
{
public:
#define ATTR(type, name, defaultVal) DECLARE_ATTRIBUTE(CGenericWeaponAtts_Core, type, name, defaultVal)
	ATTR(WeaponId_e, Id, WeaponId_e::WeaponNone);
	ATTR(const char*, Classname, NULL);
	ATTR(CBasePlayerWeapon*, ClientPredictionWeapon, NULL);
	ATTR(int, Flags, 0);
	ATTR(int, SwitchWeight, 0);

	ATTR(const CAmmoDef*, PrimaryAmmoDef, NULL);
	ATTR(const char*, PrimaryAmmoClassname, NULL);
	ATTR(const CAmmoDef*, SecondaryAmmoDef, NULL);
	ATTR(const char*, SecondaryAmmoClassname, NULL);
	ATTR(int, PrimaryAmmoOnFirstPickup, 0);
	ATTR(int, MaxClip, 0);
	ATTR(bool, AutoReload, false);
	ATTR(bool, UsesSpecialReload, false);

	ATTR(const char*, ViewModelName, NULL);
	ATTR(const char*, PlayerModelName, NULL);
	ATTR(const char*, WorldModelName, NULL);
	ATTR(const char*, PickupSoundOverride, NULL);
#undef ATTR

	inline void Validate() const
	{
		ASSERTSZ_Q(m_Classname, "Weapon must have a classname.");
		ASSERTSZ_Q(m_Id != WeaponId_e::WeaponNone, "Weapon must have a valid ID.");
		ASSERTSZ_Q(m_ViewModelName, "Weapon must have a view model.");
		ASSERTSZ_Q(!m_PrimaryAmmoDef || m_PrimaryAmmoClassname, "Weapon must specify primary ammo classname.");
		ASSERTSZ_Q(!m_SecondaryAmmoDef || m_SecondaryAmmoClassname, "Weapon must specify secondary ammo classname.");
	}
};