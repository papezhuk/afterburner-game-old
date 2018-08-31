#include "weaponregistry.h"
#include "genericweaponattributes.h"
#include "standard_includes.h"
#include "genericweapon.h"

CWeaponRegistry CWeaponRegistry::StaticInstance = CWeaponRegistry();

CWeaponRegistry::CWeaponRegistry()
	: m_AttributesList{0}
{
}

void CWeaponRegistry::Add(const CGenericWeaponAttributes* atts)
{
	if ( !atts )
	{
		return;
	}

	// Since weapon IDs are sequential, we can resize the vector to cater for the current ID.
	// Lower IDs that are added will just slot into the lower indices.
	const int id = static_cast<const int>(atts->Core().Id());
	ASSERTSZ(id >= 0 && id < MAX_WEAPONS, "Weapon ID is out of range!");
	ASSERTSZ(m_AttributesList[id] == NULL, "Attributes already present at this index.");

	m_AttributesList[id] = atts;

	// Keep track
}

const CGenericWeaponAttributes* CWeaponRegistry::Get(int index) const
{
	// Don't allow index 0 as this indicates no weapon.
	return (index > 0 && index < MAX_WEAPONS) ? m_AttributesList[index] : NULL;
}

const CGenericWeaponAttributes* CWeaponRegistry::Get(WeaponId_e id) const
{
	return Get(static_cast<int>(id));
}
