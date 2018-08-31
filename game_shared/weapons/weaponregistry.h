#pragma once
#include <cstdint>
#include "weaponids.h"
#include "cdll_dll.h"

class CGenericWeaponAttributes;
class CGenericWeapon;

class CWeaponRegistry
{
public:
	CWeaponRegistry();

	void Add(const CGenericWeaponAttributes* atts);
	const CGenericWeaponAttributes* Get(int index) const;
	const CGenericWeaponAttributes* Get(WeaponId_e id) const;

	template<typename T>
	void ForEach(const T& callback)
	{
		for ( uint32_t index = 0; index < MAX_WEAPONS; ++index )
		{
			const CGenericWeaponAttributes* atts = m_AttributesList[index];

			// Note that index 0 (Weapon_None) will not have a valid pointer.
			if ( atts )
			{
				callback(*atts);
			}
		}
	}

	static CWeaponRegistry StaticInstance;

private:
	const CGenericWeaponAttributes* m_AttributesList[MAX_WEAPONS];
};
