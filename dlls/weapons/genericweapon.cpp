#include "genericweapon.h"

void CGenericWeapon::Spawn()
{
	const CGenericWeaponAttributes atts = WeaponAttributes();
	const CGenericWeaponAtts_Core& core = atts.Core();

	pev->classname = MAKE_STRING( core.Classname() ); // hack to allow for old names
	Precache();
	m_iId = static_cast<int>(core.Id());
	SET_MODEL( ENT( pev ), core.WorldModelName() );

	m_iDefaultAmmo = core.AmmoOnFirstPickup();

	FallInit();// get ready to fall down.
}
