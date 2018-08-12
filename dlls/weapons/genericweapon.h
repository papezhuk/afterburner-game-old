#pragma once

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include <memory>

enum class e_WeaponId
{
	WeaponNone = 0,
	WeaponGenericTest
};

#define BASE_ATTR(outerClass, type, name, defaultVal) \
private: \
	type m_##name = defaultVal; \
public: \
	inline type name() const { return m_##name; } \
	inline outerClass& name(type val) { m_##name = val; return *this; }

class CGenericWeaponAtts_Core
{
public:
#define ATTR(type, name, defaultVal) BASE_ATTR(CGenericWeaponAtts_Core, type, name, defaultVal)
	ATTR(e_WeaponId, Id, e_WeaponId::WeaponNone);
	ATTR(const char*, Classname, NULL);
	ATTR(int, WeaponSlot, -1);
	ATTR(int, WeaponSlotPosition, -1);
	ATTR(int, Flags, 0);
	ATTR(int, SwitchWeight, 0);
	ATTR(int, AmmoOnFirstPickup, 0);
	ATTR(int, MaxClip, 0);
	ATTR(const char*, ViewModelName, NULL);
	ATTR(const char*, PlayerModelName, NULL);
	ATTR(const char*, WorldModelName, NULL);
#undef ATTR
};

class CGenericWeaponAtts_BaseFireMode
{
public:
	enum e_FireMode
	{
		Hitscan = 0
	};

	virtual e_FireMode Id() const = 0;
	virtual CGenericWeaponAtts_BaseFireMode* Clone() const = 0;

	template<typename T>
	T* Mode()
	{
		return dynamic_cast<T*>(this);
	}

	template<typename T>
	const T* Mode() const
	{
		return dynamic_cast<const T*>(this);
	}
};

class CGenericWeaponAtts_HitscanFireMode : public CGenericWeaponAtts_BaseFireMode
{
public:
	virtual e_FireMode Id() const override { return CGenericWeaponAtts_BaseFireMode::e_FireMode::Hitscan; }
	virtual CGenericWeaponAtts_BaseFireMode* Clone() const override { return new CGenericWeaponAtts_HitscanFireMode(*this); }

#define ATTR(type, name, defaultVal) BASE_ATTR(CGenericWeaponAtts_HitscanFireMode, type, name, defaultVal)
	ATTR(float, FireRate, 1.0f);	// Cycles per second
	ATTR(uint8_t, BulletsPerShot, 1);
	ATTR(float, Accuracy, 0.01f);
	ATTR(bool, FullAuto, false);
	ATTR(float, AutoAim, 0.0f);
	ATTR(int, Volume, NORMAL_GUN_VOLUME);
	ATTR(int, MuzzleFlashBrightness, NORMAL_GUN_FLASH);
#undef ATTR
};

#undef BASE_ATTR

class CGenericWeaponAttributes
{
public:
	CGenericWeaponAttributes(const CGenericWeaponAtts_Core& core)
		: m_Core(core),
		  m_FireModes{}
	{
	}

	CGenericWeaponAttributes(const CGenericWeaponAttributes& other)
		: m_Core(other.m_Core),
		  m_FireModes{}
	{
		for ( int mode = 0; mode < 2; ++mode )
		{
			CGenericWeaponAtts_BaseFireMode* modePtr = other.m_FireModes[mode].get();
			if ( modePtr )
			{
				m_FireModes[mode].reset(modePtr->Clone());
			}
		}
	}

	const CGenericWeaponAtts_Core& Core() const
	{
		return m_Core;
	}

	const CGenericWeaponAtts_BaseFireMode* FireMode(uint8_t mode) const
	{
		return mode < 2 ? m_FireModes[mode].get() : NULL;
	}

	CGenericWeaponAttributes& FireMode(uint8_t mode, CGenericWeaponAtts_BaseFireMode* value)
	{
		BaseFireModePtr ptr(value);

		if ( mode < 2 )
		{
			m_FireModes[mode].swap(ptr);
		}

		return *this;
	}

private:
	typedef std::unique_ptr<CGenericWeaponAtts_BaseFireMode> BaseFireModePtr;

	CGenericWeaponAtts_Core m_Core;
	BaseFireModePtr m_FireModes[2];
};

// Interface for generic weapon commands.
// Ideally we'd build a class that doesn't depend on CBasePlayerWeapon,
// but CBasePlayerWeapon is tied heavily to other engine functionality
// that it'd be non-trivial to extricate it. Perhaps we will one day?
// Until that time, Nightfire weapons should call functions through
// this interface, which CGenericWeapon will translate as an adapter
// to CBasePlayerWeapon. The reason for this is that CBasePlayerWeapon
// is quite loose with the way it manages weapon functionality -
// for example, ammo types are set when adding ammo to the weapon,
// which means a weapon that uses one type of ammo could just have it
// set to a new type on a whim. This interface is an attempt to solidify
// the functionality.
class IGenericWeapon
{
public:
	virtual ~IGenericWeapon() {}

	virtual const CGenericWeaponAttributes& WeaponAttributes() const = 0;
};

// Use on leaf classes (ie. the actual weapons that are linked to entities).
template<typename T>
class IGenericWeaponStatics : public virtual IGenericWeapon
{
public:
	virtual ~IGenericWeaponStatics() {}

	virtual const CGenericWeaponAttributes& WeaponAttributes() const override
	{
		return StaticWeaponAttributes();
	}

	const CGenericWeaponAttributes& StaticWeaponAttributes() const
	{
		// This must be implemented on the subclass.
		return T::m_StaticWeaponAttributes;
	}
};

// Build on top of CBasePlayerWeapon, because this is so tied into the engine
// already it'd be a pain to replace it (at least at this stage).
class CGenericWeapon : public CBasePlayerWeapon,
					   public virtual IGenericWeapon
{
public:
	void Spawn() override;
};
