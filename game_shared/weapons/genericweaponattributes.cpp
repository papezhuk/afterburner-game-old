#include "genericweaponattributes.h"
#include "weaponregistry.h"

void CGenericWeaponAttributes::Register()
{
	m_Core.Validate();
	SetFireModeSignatures();
	CWeaponRegistry::StaticInstance().Add(this);
}
