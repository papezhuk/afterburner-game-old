#include "weaponregistry.h"
#include "genericweaponattributes.h"
#include "standard_includes.h"
#include "genericweapon.h"

CWeaponRegistry CWeaponRegistry::StaticInstance = CWeaponRegistry();

void CWeaponRegistry::Add(const CGenericWeaponAttributes* atts)
{
	if ( !atts )
	{
		return;
	}

	// Since weapon IDs are sequential, we can resize the vector to cater for the current ID.
	// Lower IDs that are added will just slot into the lower indices.
	const int id = static_cast<const int>(atts->Core().Id());
	const size_t requiredLength = id + 1;
	ResizePtrVectorToFit(m_AttributesList, requiredLength);

	ASSERTSZ(m_AttributesList[id] == NULL, "Attributes already present at this index.");

	m_AttributesList[id] = atts;
}

const CGenericWeaponAttributes* CWeaponRegistry::Get(int index) const
{
	return (index > 0 && index < m_AttributesList.size()) ? m_AttributesList[index] : NULL;
}

const CGenericWeaponAttributes* CWeaponRegistry::Get(WeaponId_e id) const
{
	return Get(static_cast<int>(id));
}

size_t CWeaponRegistry::Count() const
{
	return m_AttributesList.size();
}
