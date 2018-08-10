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

enum class e_WeaponClip
{
	None = -1,
	Clip0 = 0,
	Clip1
};

#define BASE_ATTR(outerClass, type, name, defaultVal) \
private: \
	type m_##name = defaultVal; \
public: \
	inline type name() const { return m_##name; } \
	inline outerClass& name(const type& val) { m_##name = val; return *this; }

#define CORE_ATTR(type, name, defaultVal) BASE_ATTR(CGenericWeaponCoreData, type, name, defaultVal)

class CGenericWeaponCoreData
{
public:
	CORE_ATTR(e_WeaponId, WeaponId, e_WeaponId::WeaponNone);
	CORE_ATTR(const char*, WeaponClassname, NULL);
};

#undef CORE_ATTR
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
	WEAPON_ATTR(e_WeaponClip, PrimaryClip, e_WeaponClip::Clip0);

	WEAPON_ATTR(bool, HasSecondaryFireMode, false);
	WEAPON_ATTR(float, SecondaryFireRate, 3.33f);	// Rounds/sec
	WEAPON_ATTR(float, SecondaryAccuracy, 0.01f);
	WEAPON_ATTR(bool, SecondaryIsAutomatic, false);
	WEAPON_ATTR(float, SecondaryAutoAim, AUTOAIM_10DEGREES);
	WEAPON_ATTR(int, SecondaryVolume, NORMAL_GUN_VOLUME);
	WEAPON_ATTR(int, SecondaryMuzzleFlash, NORMAL_GUN_FLASH);
	WEAPON_ATTR(uint8_t, SecondaryRoundsPerShot, 1);
	WEAPON_ATTR(e_WeaponClip, SecondaryClip, e_WeaponClip::None);

	// Weapon clips
	WEAPON_ATTR(uint8_t, Clip0Size, 8);
	WEAPON_ATTR(const char*, Clip0AmmoName, NULL);

	WEAPON_ATTR(uint8_t, Clip1Size, 8);
	WEAPON_ATTR(const char*, Clip1AmmoName, NULL);
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

class CGenericWeapon : public CBasePlayerItem
{
public:
	// Dynamic versions of IGenericWeaponStatics functions.
	virtual const CGenericWeaponAttributes& WeaponAttributes() const = 0;

	// Return TRUE if the weapon should be added to the player.
	// The default implementation just returns TRUE.
	virtual int AddWeapon(void);

	// Base overrides
	virtual int AddToPlayer(CBasePlayer *pPlayer) override;
	virtual int AddDuplicate(CBasePlayerItem *pItem) override;

	void SendWeaponAnim(int iAnim, int body = 0);

	inline BOOL UseDecrement() const
	{
#if defined( CLIENT_WEAPONS )
		return TRUE;
#else
		return FALSE;
#endif
	}

private:
	// Ammo indexes that correspond to the ammo this weapon uses.
	int m_iClip0AmmoType;
	int m_iClip1AmmoType;
};
