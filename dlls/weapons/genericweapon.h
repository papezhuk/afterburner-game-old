#pragma once

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"

enum class e_WeaponId
{
	WeaponNone = 0
};

#define BASE_ATTR(outerClass, type, name, defaultVal) \
private: \
	type m_##name = defaultVal; \
public: \
	inline type name() const { return m_##name; } \
	inline outerClass& name(const type& val) { m_##name = val; return *this; }

#define WEAPON_ATTR(type, name, defaultVal) BASE_ATTR(CGenericWeaponAttributes, type, name, defaultVal)

class CGenericWeaponAttributes
{
public:
	// Primary and secondary firing modes.
	WEAPON_ATTR(float, PrimaryFireRate, 3.33f);	// Rounds/sec
	WEAPON_ATTR(float, PrimaryAccuracy, 0.01f);
	WEAPON_ATTR(bool, PrimaryIsAutomatic, false);
	WEAPON_ATTR(float, PrimaryAutoAim, AUTOAIM_10DEGREES);
	WEAPON_ATTR(int, PrimaryVolume, NORMAL_GUN_VOLUME);
	WEAPON_ATTR(int, PrimaryMuzzleFlash, NORMAL_GUN_FLASH);
	WEAPON_ATTR(uint8_t, PrimaryRoundsPerShot, 1);

	WEAPON_ATTR(bool, HasSecondaryFireMode, false);
	WEAPON_ATTR(float, SecondaryFireRate, 3.33f);	// Rounds/sec
	WEAPON_ATTR(float, SecondaryAccuracy, 0.01f);
	WEAPON_ATTR(bool, SecondaryIsAutomatic, false);
	WEAPON_ATTR(float, SecondaryAutoAim, AUTOAIM_10DEGREES);
	WEAPON_ATTR(int, SecondaryVolume, NORMAL_GUN_VOLUME);
	WEAPON_ATTR(int, SecondaryMuzzleFlash, NORMAL_GUN_FLASH);
	WEAPON_ATTR(uint8_t, SecondaryRoundsPerShot, 1);
};

#undef WEAPON_ATTR
#undef BASE_ATTR

// Use on leaf classes (ie. the actual weapons that are linked to entities).
template<typename T>
class IGenericWeaponStatics
{
public:
	virtual ~IGenericWeaponStatics() {}

	const CGenericWeaponAttributes& StaticWeaponAttributes() const
	{
		return T::m_StaticWeaponAttributes;
	}
};

// Build on top of CBasePlayerWeapon, because this is so tied into the engine
// already it'd be a pain to replace it (at least at this stage).
class CGenericWeapon : public CBasePlayerWeapon
{
public:
};
