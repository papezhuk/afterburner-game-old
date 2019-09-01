#pragma once

#include "weaponatts_base.h"

typedef struct CAmmoDef;

namespace WeaponAtts
{
	struct CWAAmmoDef : public CWABase
	{
		const CAmmoDef* PrimaryAmmoDef = nullptr;
		const char* PrimaryAmmoClassname = nullptr;
		const CAmmoDef* SecondaryAmmoDef = nullptr;
		const char* SecondaryAmmoClassname = nullptr;
		int PrimaryAmmoOnFirstPickup = 0;
	};
}
