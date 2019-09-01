#pragma once

#include <cstring>
#include "weaponatts_base.h"
#include "const.h"
#include "weightedvaluelist.h"

namespace WeaponAtts
{
	struct CWASoundSet : public CWABase
	{
		float MinVolume = 1.0f;
		float MaxVolume = 1.0f;
		int MinPitch = 100;
		int MaxPitch = 100;
		float Attenuation = ATTN_NORM;
		int Flags = 0;

		// Names are expected to be compile-time constant!
		WeightedValueList<const char*> SoundNames;
	};
}
