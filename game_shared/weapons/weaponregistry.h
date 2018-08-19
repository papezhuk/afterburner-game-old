#pragma once
#include "weaponids.h"
#include <vector>

class CGenericWeaponAttributes;
class CGenericWeapon;

class CWeaponRegistry
{
public:
	void Add(const CGenericWeaponAttributes* atts);
	const CGenericWeaponAttributes* Get(int index) const;
	const CGenericWeaponAttributes* Get(WeaponId_e id) const;
	size_t Count() const;

	template<typename T>
	void ForEach(const T& callback)
	{
		for ( const CGenericWeaponAttributes* atts : m_AttributesList )
		{
			// Note that index 0 (Weapon_None) will not have a valid pointer.
			if ( atts )
			{
				callback(*atts);
			}
		}
	}

	static CWeaponRegistry StaticInstance;

private:
	template<typename T>
	void ResizePtrVectorToFit(std::vector<T*>& vector, size_t minSize)
	{
		if ( vector.size() < minSize )
		{
			vector.reserve(minSize);

			do
			{
				vector.push_back(0);
			}
			while ( vector.size() < minSize );
		}
	}

	std::vector<const CGenericWeaponAttributes*> m_AttributesList;
};
