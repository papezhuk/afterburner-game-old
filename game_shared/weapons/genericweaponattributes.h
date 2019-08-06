#pragma once

#include <memory>

#include "standard_includes.h"
#include "weapons.h"
#include "weaponids.h"
#include "ammodefs.h"
#include "skill.h"
#include "utlstring.h"
#include "utlvector.h"

#ifndef CLIENT_DLL
#include "botweaponattributes.h"
#endif

class CGenericWeapon;

template<typename T>
class WeightedValueList
{
public:
	uint32_t Add(const T& value, float weight, bool normalise = true)
	{
		m_Items.AddToTail({value, weight, 0.0f});

		if ( normalise )
		{
			NormaliseWeights();
		}

		return m_Items.Count();
	}

	const T& Value(uint32_t index) const
	{
		ASSERTSZ_Q((int)index > 0 && (int)index < m_Items.Count(), "Index must be in range.");
		return m_Items[index].m_Value;
	}

	// Given a float between 0 and 1 (assumed to be randomly generated),
	// returns the index of a value taking into account all weights.
	// Assumes NormaliseWeight() has been called.
	uint32_t IndexByProbabilisticValue(float input) const
	{
		float accumulatedWeight = 0;

		for ( int index = 0; index < m_Items.Count(); ++index )
		{
			// accumulatedWeight is the lower bound in the [0 1] range,
			// and this + the item's normalised weight is the upper bound.
			// If the input is within these bounds, this it the item we choose.
			if ( accumulatedWeight + m_Items[index].m_NormalisedWeight >= input )
			{
				return index;
			}

			accumulatedWeight += m_Items[index].m_NormalisedWeight;
		}

		// Unlikely to happen - probably the result of rounding errors.
		return m_Items.Count() - 1;
	}

	// As above, but returns the item pointed to by the index.
	const T& ItemByProbabilisticValue(float input) const
	{
		ASSERTSZ(m_Items.Count() > 0, "Container must not be empty.");
		return m_Items[IndexByProbabilisticValue(input)].m_Value;
	}

	float OriginalWeight(uint32_t index) const
	{
		ASSERTSZ(index < m_Items.Count(), "Index must be in range.");
		return m_Items[index].m_Weight;
	}

	float NormalisedWeight(uint32_t index) const
	{
		ASSERTSZ(index < m_Items.Count(), "Index must be in range.");
		return m_Items[index].m_NormalisedWeight;
	}

	size_t Count() const
	{
		return m_Items.Count();
	}

	void NormaliseWeights()
	{
		// We calculate this each time, because if we kept track of it
		// on addition/deletion floating point errors could accumulate.
		float totalWeight = 0.0f;

		for ( int index = 0; index < m_Items.Count(); ++index )
		{
			totalWeight += m_Items[index].m_Weight;
		}

		for ( int index = 0; index < m_Items.Count(); ++index )
		{
			m_Items[index].m_NormalisedWeight = m_Items[index].m_Weight / totalWeight;
		}
	}

private:
	struct WeightedValue
	{
		T m_Value;
		float m_Weight;
		float m_NormalisedWeight;
	};

	CUtlVector<WeightedValue> m_Items;
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
		ASSERTSZ_Q(m_PlayerModelName, "Weapon must have a player model.");
		ASSERTSZ_Q(m_WorldModelName, "Weapon must have a world model.");
		ASSERTSZ_Q(!m_PrimaryAmmoDef || m_PrimaryAmmoClassname, "Weapon must specify primary ammo classname.");
		ASSERTSZ_Q(!m_SecondaryAmmoDef || m_SecondaryAmmoClassname, "Weapon must specify secondary ammo classname.");
	}
};

class CGenericWeaponAttributes_SkillRecord
{
public:
	typedef float skilldata_t::* SkillDataEntryPtr;

	CGenericWeaponAttributes_SkillRecord(const char* cvarBaseName, SkillDataEntryPtr entry);
	CGenericWeaponAttributes_SkillRecord(const CGenericWeaponAttributes_SkillRecord& other);

	// After registring cvars, the instance of the class must persist until shutdown.
	// This class is designed to be used statically, so it should be OK.
	void RegisterCvars() const;
	void UpdateSkillValue(skilldata_t* instance) const;

private:
	SkillDataEntryPtr m_Entry;
	CUtlString m_BaseName;
	CUtlString m_NameBuffers[TOTAL_SKILL_LEVELS];
	mutable cvar_t m_Cvars[TOTAL_SKILL_LEVELS];
};

class CGenericWeaponAttributes_Skill
{
public:
	CGenericWeaponAttributes_Skill();
	CGenericWeaponAttributes_Skill(const CGenericWeaponAttributes_Skill& other);
	CGenericWeaponAttributes_Skill& operator =(const CGenericWeaponAttributes_Skill& other);

	inline CGenericWeaponAttributes_Skill& Record(const CGenericWeaponAttributes_SkillRecord& record)
	{
		m_Records.AddToTail(record);
		return *this;
	}

	inline CGenericWeaponAttributes_Skill& Record(const char* cvarBaseName, CGenericWeaponAttributes_SkillRecord::SkillDataEntryPtr entry)
	{
		m_Records.AddToTail(CGenericWeaponAttributes_SkillRecord(cvarBaseName, entry));
		return *this;
	}

	inline void RegisterCvars() const
	{
		FOR_EACH_VEC(m_Records, index)
		{
			m_Records[index].RegisterCvars();
		}
	}

	inline void UpdateSkillValues(skilldata_t* instance) const
	{
		FOR_EACH_VEC(m_Records, index)
		{
			m_Records[index].UpdateSkillValue(instance);
		}
	}

private:
	CUtlVector<CGenericWeaponAttributes_SkillRecord> m_Records;
};

class CGenericWeaponAttributes_Sound
{
public:
#define ATTR(type, name, defaultVal) BASE_ATTR(CGenericWeaponAttributes_Sound, type, name, defaultVal)
	ATTR(float, MinVolume, 1.0f);
	ATTR(float, MaxVolume, 1.0f);
	ATTR(int, MinPitch, 100);
	ATTR(int, MaxPitch, 100);
	ATTR(float, Attenuation, ATTN_NORM);
	ATTR(int, Flags, 0);
#undef ATTR

	inline CGenericWeaponAttributes_Sound& Sound(const char* name, float weight = 1.0f)
	{
		m_SoundNames.Add(name, weight);
		return *this;
	}

	inline const WeightedValueList<const char*>& SoundList() const
	{
		return m_SoundNames;
	}

private:
	// Expects static char pointers fixed at compile time!
	WeightedValueList<const char*> m_SoundNames;
};

class CGenericWeaponAtts_BaseFireMechanic
{
public:
	enum FireMechanic_e
	{
		Hitscan = 0,
		Projectile
	};

	virtual ~CGenericWeaponAtts_BaseFireMechanic() {}

	virtual FireMechanic_e Id() const = 0;
	virtual CGenericWeaponAtts_BaseFireMechanic* Clone() const = 0;

	template<typename T>
	T* AsType()
	{
		return dynamic_cast<T*>(this);
	}

	template<typename T>
	const T* AsType() const
	{
		return dynamic_cast<const T*>(this);
	}
};

class CGenericWeaponAtts_HitscanFireMechanic : public CGenericWeaponAtts_BaseFireMechanic
{
public:
	typedef CGenericWeaponAttributes_SkillRecord::SkillDataEntryPtr SkillBasedDamagePtr;

	virtual ~CGenericWeaponAtts_HitscanFireMechanic() {}

	virtual FireMechanic_e Id() const override { return CGenericWeaponAtts_BaseFireMechanic::FireMechanic_e::Hitscan; }
	virtual CGenericWeaponAtts_BaseFireMechanic* Clone() const override { return new CGenericWeaponAtts_HitscanFireMechanic(*this); }

#define ATTR(type, name, defaultVal) BASE_ATTR(CGenericWeaponAtts_HitscanFireMechanic, type, name, defaultVal)
	ATTR(uint8_t, BulletsPerShot, 1);
	ATTR(SkillBasedDamagePtr, BaseDamagePerShot, NULL);
	ATTR(float, AutoAim, 0.0f);
	ATTR(const char*, ShellModelName, NULL);
#undef ATTR
};

class CGenericWeaponAtts_ProjectileFireMechanic : public CGenericWeaponAtts_BaseFireMechanic
{
public:
	virtual ~CGenericWeaponAtts_ProjectileFireMechanic() {}

	virtual FireMechanic_e Id() const override { return CGenericWeaponAtts_BaseFireMechanic::FireMechanic_e::Projectile; }
	virtual CGenericWeaponAtts_BaseFireMechanic* Clone() const override { return new CGenericWeaponAtts_ProjectileFireMechanic(*this); }

#define ATTR(type, name, defaultVal) BASE_ATTR(CGenericWeaponAtts_ProjectileFireMechanic, type, name, defaultVal)
	// Nothing yet
#undef ATTR
};

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

#define ATTR(type, name, defaultVal) BASE_ATTR(CGenericWeaponAtts_FireMode, type, name, defaultVal)
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

class CGenericWeaponAtts_Animations
{
public:
#define ATTR(type, name, defaultVal) BASE_ATTR(CGenericWeaponAtts_Animations, type, name, defaultVal)
	ATTR(const char*, Extension, NULL);
	ATTR(int, Index_Draw, -1);
	ATTR(int, Index_ReloadWhenEmpty, -1);		// If left at -1, ReloadWhenNotEmpty is used instead.
	ATTR(int, Index_ReloadWhenNotEmpty, -1);
#undef ATTR

	inline const CGenericWeaponAttributes_Sound& ReloadSounds() const
	{
		return m_ReloadSounds;
	}

	inline CGenericWeaponAtts_Animations& ReloadSounds(const CGenericWeaponAttributes_Sound& sound)
	{
		m_ReloadSounds = sound;
		return *this;
	}

	inline bool HasSounds() const
	{
		return m_ReloadSounds.SoundList().Count() > 0;
	}

private:
	CGenericWeaponAttributes_Sound m_ReloadSounds;
};

class CGenericWeaponAtts_IdleAnimations
{
public:
#define ATTR(type, name, defaultVal) BASE_ATTR(CGenericWeaponAtts_IdleAnimations, type, name, defaultVal)
	ATTR(bool, IdleWhenClipEmpty, false);	// Usually idle animations have weapon in a loaded state.
#undef ATTR

	inline CGenericWeaponAtts_IdleAnimations& Animation(int index, float weight = 1.0f)
	{
		m_AnimIndices.Add(index, weight);
		return *this;
	}

	inline const WeightedValueList<int>& List() const
	{
		return m_AnimIndices;
	}

private:
	WeightedValueList<int> m_AnimIndices;
};

#undef BASE_ATTR

class CGenericWeaponAttributes
{
public:
	CGenericWeaponAttributes(const CGenericWeaponAtts_Core& core)
		: m_Core(core),
		  m_Skill(),
		  m_NewFireModes{},
		  m_Animations(),
		  m_IdleAnimations()
	{
	}

	CGenericWeaponAttributes(const CGenericWeaponAttributes& other)
		: m_Core(other.m_Core),
		  m_Skill(other.m_Skill),
		  m_NewFireModes{},
		  m_Animations(),
		  m_IdleAnimations()
	{
		m_Animations = other.m_Animations;
		m_IdleAnimations = other.m_IdleAnimations;

		for ( int mode = 0; mode < 2; ++mode )
		{
			m_NewFireModes[mode] = other.m_NewFireModes[mode];
		}

		Register();
	}

	inline CGenericWeaponAttributes& operator =(const CGenericWeaponAttributes& other)
	{
		m_Core = other.m_Core;
		m_Animations = other.m_Animations;
		m_IdleAnimations = other.m_IdleAnimations;

		for ( int mode = 0; mode < 2; ++mode )
		{
			m_NewFireModes[mode] = other.m_NewFireModes[mode];
		}

		Register();
		return *this;
	}

	// Implemented in .cpp to remove cyclic dependencies.
	void Register();

	inline const CGenericWeaponAtts_Core& Core() const
	{
		return m_Core;
	}

	inline const CGenericWeaponAttributes_Skill& Skill() const
	{
		return m_Skill;
	}

	inline const CGenericWeaponAtts_Animations& Animations() const
	{
		return m_Animations;
	}

	inline const CGenericWeaponAtts_IdleAnimations& IdleAnimations() const
	{
		return m_IdleAnimations;
	}

	inline CGenericWeaponAttributes& Skill(const CGenericWeaponAttributes_Skill& skill)
	{
		m_Skill = skill;
		return *this;
	}

	inline CGenericWeaponAttributes& Animations(const CGenericWeaponAtts_Animations& anims)
	{
		m_Animations = anims;
		return *this;
	}

	inline CGenericWeaponAttributes& IdleAnimations(const CGenericWeaponAtts_IdleAnimations& anims)
	{
		m_IdleAnimations = anims;
		return *this;
	}

	inline const CGenericWeaponAtts_FireMode& FireMode(uint8_t mode) const
	{
		static const CGenericWeaponAtts_FireMode dummy;

		if ( mode < 0 || mode > 1 )
		{
			ASSERTSZ_Q(false, "Invalid fire mode index");
			return dummy;
		}

		return m_NewFireModes[mode];
	}

	inline CGenericWeaponAttributes& FireMode(uint8_t mode, const CGenericWeaponAtts_FireMode fireMode)
	{
		if ( mode < 0 || mode > 1 )
		{
			ASSERTSZ_Q(false, "Invalid fire mode index");
			return *this;
		}

		m_NewFireModes[mode] = fireMode;
		return *this;
	}

#ifndef CLIENT_DLL
	inline const CBotWeaponAttributes& BotWeaponAttributes() const
	{
		return m_BotWeaponAttributes;
	}

	inline CGenericWeaponAttributes& BotWeaponAttributes(const CBotWeaponAttributes& val)
	{
		m_BotWeaponAttributes = val;
		return *this;
	}
#endif

private:
	inline void SetFireModeSignatures()
	{
		for ( int index = 0; index < 2; ++index )
		{
			m_NewFireModes[index].SetSignature(m_Core.Id(), index);
		}
	}

	CGenericWeaponAtts_Core m_Core;
	CGenericWeaponAttributes_Skill m_Skill;
	CGenericWeaponAtts_Animations m_Animations;
	CGenericWeaponAtts_IdleAnimations m_IdleAnimations;
	CGenericWeaponAtts_FireMode m_NewFireModes[2];

#ifndef CLIENT_DLL
	CBotWeaponAttributes m_BotWeaponAttributes;
#endif
};
