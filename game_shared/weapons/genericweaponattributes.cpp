#include "genericweaponattributes.h"
#include "weaponregistry.h"

void CGenericWeaponAttributes::Register()
{
	CWeaponRegistry::StaticInstance().Add(this);
}
