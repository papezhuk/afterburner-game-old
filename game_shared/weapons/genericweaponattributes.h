#pragma once

#include <memory>
#include <vector>

#include "standard_includes.h"
#include "weapons.h"
#include "weaponids.h"

template<typename T>
class WeightedValueList
{
public:
	uint32_t Add(const T& value, float weight, bool normalise = true)
	{
		m_Items.push_back({value, weight, 0.0f});

		if ( normalise )
		{
			NormaliseWeights();
		}

		return m_Items.size();
	}

	const T& Value(uint32_t index) const
	{
		ASSERTSZ(index < m_Items.size(), "Index must be in range.");
		return m_Items[index].m_Value;
	}

	// Given a float between 0 and 1 (assumed to be randomly generated),
	// returns the index of a value taking into account all weights.
	// Assumes NormaliseWeight() has been called.
	uint32_t IndexByProbabilisticValue(float input) const
	{
		float accumulatedWeight = 0;

		for ( uint32_t index = 0; index < m_Items.size(); ++index )
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
		return m_Items.size() - 1;
	}

	float OriginalWeight(uint32_t index) const
	{
		ASSERTSZ(index < m_Items.size(), "Index must be in range.");
		return m_Items[index].m_Weight;
	}

	float NormalisedWeight(uint32_t index) const
	{
		ASSERTSZ(index < m_Items.size(), "Index must be in range.");
		return m_Items[index].m_NormalisedWeight;
	}

	size_t Count() const
	{
		return m_Items.size();
	}

	void NormaliseWeights()
	{
		// We calculate this each time, because if we kept track of it
		// on addition/deletion floating point errors could accumulate.
		float totalWeight = 0.0f;

		for ( uint32_t index = 0; index < m_Items.size(); ++index )
		{
			totalWeight += m_Items[index].m_Weight;
		}

		for ( uint32_t index = 0; index < m_Items.size(); ++index )
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

	std::vector<WeightedValue> m_Items;
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
	ATTR(int, WeaponSlot, -1);
	ATTR(int, WeaponSlotPosition, -1);
	ATTR(int, Flags, 0);
	ATTR(int, SwitchWeight, 0);

	ATTR(const char*, PrimaryAmmoName, NULL);
	ATTR(int, PrimaryAmmoMax, -1);
	ATTR(int, PrimaryAmmoOnFirstPickup, 0);

	ATTR(const char*, SecondaryAmmoName, NULL);
	ATTR(int, SecondaryAmmoMax, -1);

	ATTR(int, MaxClip, 0);

	ATTR(const char*, ViewModelName, NULL);
	ATTR(const char*, PlayerModelName, NULL);
	ATTR(const char*, WorldModelName, NULL);
#undef ATTR

	inline void Validate() const
	{
		ASSERTSZ(m_Classname, "Weapon must have a classname.");
		ASSERTSZ(m_Id != WeaponId_e::WeaponNone, "Weapon must have a valid ID.");
		ASSERTSZ(m_ViewModelName, "Weapon must have a view model.");
		ASSERTSZ(m_PlayerModelName, "Weapon must have a player model.");
		ASSERTSZ(m_WorldModelName, "Weapon must have a world model.");
	}
};

class CGenericWeaponAtts_BaseFireMode
{
public:
	enum e_FireMode
	{
		Hitscan = 0
	};

	CGenericWeaponAtts_BaseFireMode(const char* eventName)
		: m_szEventName(eventName)
	{
	}

	virtual e_FireMode Id() const = 0;
	virtual CGenericWeaponAtts_BaseFireMode* Clone() const = 0;

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

	const char* EventName() const
	{
		return m_szEventName;
	}

private:
	const char* m_szEventName;
};

class CGenericWeaponAtts_HitscanFireMode : public CGenericWeaponAtts_BaseFireMode
{
public:
	CGenericWeaponAtts_HitscanFireMode(const char* eventName)
		: CGenericWeaponAtts_BaseFireMode(eventName)
	{
	}

	virtual e_FireMode Id() const override { return CGenericWeaponAtts_BaseFireMode::e_FireMode::Hitscan; }
	virtual CGenericWeaponAtts_BaseFireMode* Clone() const override { return new CGenericWeaponAtts_HitscanFireMode(*this); }

#define ATTR(type, name, defaultVal) BASE_ATTR(CGenericWeaponAtts_HitscanFireMode, type, name, defaultVal)
	ATTR(float, FireRate, 1.0f);	// Cycles per second
	ATTR(uint8_t, BulletsPerShot, 1);
	ATTR(Bullet, BulletType, BULLET_NONE);
	ATTR(float, Spread, 0.01f);
	ATTR(bool, FullAuto, false);
	ATTR(float, AutoAim, 0.0f);
	ATTR(int, Volume, NORMAL_GUN_VOLUME);
	ATTR(int, MuzzleFlashBrightness, NORMAL_GUN_FLASH);
#undef ATTR
};

class CGenericWeaponAtts_Animations
{
public:
#define ATTR(type, name, defaultVal) BASE_ATTR(CGenericWeaponAtts_Animations, type, name, defaultVal)
	ATTR(const char*, Extension, NULL);
	ATTR(int, Index_Draw, -1);
	ATTR(int, Index_ReloadWhenEmpty, -1);
	ATTR(int, Index_ReloadWhenNotEmpty, -1);
#undef ATTR
};

class CGenericWeaponAtts_IdleAnimations
{
public:
	CGenericWeaponAtts_IdleAnimations& Animation(int index, float weight)
	{
		m_AnimIndices.Add(index, weight);
		return *this;
	}

	const WeightedValueList<int>& List() const
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
		  m_FireModes{},
		  m_Animations(),
		  m_IdleAnimations()
	{
		Register();
	}

	CGenericWeaponAttributes(const CGenericWeaponAttributes& other)
		: m_Core(other.m_Core),
		  m_FireModes{},
		  m_Animations(other.m_Animations),
		  m_IdleAnimations(other.m_IdleAnimations)
	{
		for ( int mode = 0; mode < 2; ++mode )
		{
			CGenericWeaponAtts_BaseFireMode* modePtr = other.m_FireModes[mode].get();
			if ( modePtr )
			{
				m_FireModes[mode].reset(modePtr->Clone());
			}
		}

		Register();
	}

	// Implemented in .cpp to remove cyclic dependencies.
	void Register();

	const CGenericWeaponAtts_Core& Core() const
	{
		return m_Core;
	}

	const CGenericWeaponAtts_Animations& Animations() const
	{
		return m_Animations;
	}

	const CGenericWeaponAtts_IdleAnimations& IdleAnimations() const
	{
		return m_IdleAnimations;
	}

	CGenericWeaponAttributes& Animations(const CGenericWeaponAtts_Animations& anims)
	{
		m_Animations = anims;
		return *this;
	}

	CGenericWeaponAttributes& IdleAnimations(const CGenericWeaponAtts_IdleAnimations& anims)
	{
		m_IdleAnimations = anims;
		return *this;
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
	CGenericWeaponAtts_Animations m_Animations;
	CGenericWeaponAtts_IdleAnimations m_IdleAnimations;
	BaseFireModePtr m_FireModes[2];
};