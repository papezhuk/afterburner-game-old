#pragma once

#include "weaponatts_sound.h"
#include "weightedvaluelist.h"

class CGenericWeaponAtts_Animations
{
public:
#define ATTR(type, name, defaultVal) DECLARE_ATTRIBUTE(CGenericWeaponAtts_Animations, type, name, defaultVal)
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
#define ATTR(type, name, defaultVal) DECLARE_ATTRIBUTE(CGenericWeaponAtts_IdleAnimations, type, name, defaultVal)
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