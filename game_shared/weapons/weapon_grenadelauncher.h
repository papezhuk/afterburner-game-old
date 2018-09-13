#pragma once

#include "standard_includes.h"
#include "genericweapon.h"

class CWeaponGrenadeLauncher : public CGenericWeapon
{
public:
	virtual const CGenericWeaponAttributes& WeaponAttributes() const override;
};
