#pragma once

#include "const.h"
#include "genericweaponattributedefs.h"
#include "weightedvaluelist.h"

class CGenericWeaponAttributes_Sound
{
public:
#define ATTR(type, name, defaultVal) DECLARE_ATTRIBUTE(CGenericWeaponAttributes_Sound, type, name, defaultVal)
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