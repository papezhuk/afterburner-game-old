#pragma once

#include "genericweaponattributedefs.h"
#include "weaponatts_basefiremechanic.h"

class CGenericWeaponAtts_ProjectileFireMechanic : public CGenericWeaponAtts_BaseFireMechanic
{
public:
	virtual ~CGenericWeaponAtts_ProjectileFireMechanic() {}

	virtual FireMechanic_e Id() const override { return CGenericWeaponAtts_BaseFireMechanic::FireMechanic_e::Projectile; }
	virtual CGenericWeaponAtts_BaseFireMechanic* Clone() const override { return new CGenericWeaponAtts_ProjectileFireMechanic(*this); }

#define ATTR(type, name, defaultVal) DECLARE_ATTRIBUTE(CGenericWeaponAtts_ProjectileFireMechanic, type, name, defaultVal)
	// Nothing yet
#undef ATTR
};