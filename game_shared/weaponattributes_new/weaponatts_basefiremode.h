#pragma once

#include "weaponatts_base.h"

namespace WeaponAtts
{
	struct CWABaseFireMode : public CWABase
	{
		const char* EventScript = nullptr;
		bool FiresUnderwater = false;
	};
}
