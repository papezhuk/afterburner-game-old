#pragma once

#include <cstdint>
#include "genericweaponattributedefs.h"
#include "weaponatts_basefiremechanic.h"
#include "weaponids.h"
#include "weapons.h"

static constexpr uint8_t WEAPON_MAX_FIRE_MODES = 2;

class CGenericWeaponAtts_FireMode
{
public:
	enum class AmmoType_e
	{
		None = 0,
		Primary,
		Secondary
	};

	struct FireModeSignature
	{
		WeaponId_e m_iWeaponId;
		uint8_t m_iFireMode;

		FireModeSignature()
			: m_iWeaponId(WeaponId_e::WeaponNone),
			  m_iFireMode(0)
		{
		}
	};

	CGenericWeaponAtts_FireMode()
	{
	}

	CGenericWeaponAtts_FireMode(const CGenericWeaponAtts_FireMode& other)
	{
		CopyFrom(other);
	}

	CGenericWeaponAtts_FireMode& operator =(const CGenericWeaponAtts_FireMode& other)
	{
		CopyFrom(other);
		return *this;
	}

#define ATTR(type, name, defaultVal) DECLARE_ATTRIBUTE(CGenericWeaponAtts_FireMode, type, name, defaultVal)
	ATTR(const char*, Event, NULL)
	ATTR(bool, FiresUnderwater, false);
	ATTR(AmmoType_e, UsesAmmo, AmmoType_e::None);
	ATTR(float, FireRate, 1.0f);	// Cycles per second
	ATTR(bool, FullAuto, false);
	ATTR(float, SpreadX, 0.0f);
	ATTR(float, SpreadY, 0.0f);
	ATTR(int, Volume, NORMAL_GUN_VOLUME);
	ATTR(int, MuzzleFlashBrightness, NORMAL_GUN_FLASH);
	ATTR(int, AnimIndex_FireNotEmpty, -1);
	ATTR(int, AnimIndex_FireEmpty, -1);		// If left at -1, FireNotEmpty used instead.
	ATTR(float, ViewPunchY, 0.0f);
	ATTR(int, ViewModelBodyOverride, -1);	// If specified, event uses this body index for the view model.
#undef ATTR

	inline void Validate()
	{
		ASSERTSZ_Q(m_Event, "Weapon must have a fire event specified.");
		ASSERTSZ_Q(m_AnimIndex_FireNotEmpty >= 0, "FireNotEmpty animation must be specified.");
	}

	// Convenience:
	inline CGenericWeaponAtts_FireMode& UniformSpread(float spread)
	{
		return SpreadX(spread).SpreadY(spread);
	}

	inline const CGenericWeaponAttributes_Sound& Sounds() const
	{
		return m_Sounds;
	}

	inline CGenericWeaponAtts_FireMode& Sounds(const CGenericWeaponAttributes_Sound& sound)
	{
		m_Sounds = sound;
		return *this;
	}

	inline bool HasSounds() const
	{
		return m_Sounds.SoundList().Count() > 0;
	}

	inline const CGenericWeaponAtts_BaseFireMechanic* Mechanic() const
	{
		return m_pMechanic.get();
	}

	inline CGenericWeaponAtts_FireMode& Mechanic(CGenericWeaponAtts_BaseFireMechanic* value)
	{
		BaseFireMechanicPtr ptr(value);
		m_pMechanic.swap(ptr);

		return *this;
	}

	inline bool HasMechanic() const
	{
		return m_pMechanic.get() != NULL;
	}

	inline void SetSignature(WeaponId_e weaponId, int fireMode)
	{
		m_Signature.m_iWeaponId = weaponId;
		m_Signature.m_iFireMode = fireMode;
	}

	inline const FireModeSignature* Signature() const
	{
		return &m_Signature;
	};

private:
	typedef std::unique_ptr<CGenericWeaponAtts_BaseFireMechanic> BaseFireMechanicPtr;

	inline void CopyFrom(const CGenericWeaponAtts_FireMode& other)
	{
		m_AnimIndex_FireEmpty = other.m_AnimIndex_FireEmpty;
		m_AnimIndex_FireNotEmpty = other.m_AnimIndex_FireNotEmpty;
		m_Event = other.m_Event;
		m_FireRate = other.m_FireRate;
		m_FiresUnderwater = other.m_FiresUnderwater;
		m_FullAuto = other.m_FullAuto;
		m_MuzzleFlashBrightness = other.m_MuzzleFlashBrightness;
		m_Signature = other.m_Signature;
		m_Sounds = other.m_Sounds;
		m_SpreadX = other.m_SpreadX;
		m_SpreadY = other.m_SpreadY;
		m_UsesAmmo = other.m_UsesAmmo;
		m_ViewModelBodyOverride = other.m_ViewModelBodyOverride;
		m_ViewPunchY = other.m_ViewPunchY;
		m_Volume = other.m_Volume;

		CGenericWeaponAtts_BaseFireMechanic* mechanicPtr = other.m_pMechanic.get();
		if ( mechanicPtr )
		{
			m_pMechanic.reset(mechanicPtr->Clone());
		}
	}

	BaseFireMechanicPtr m_pMechanic;
	FireModeSignature m_Signature;
	CGenericWeaponAttributes_Sound m_Sounds;
};