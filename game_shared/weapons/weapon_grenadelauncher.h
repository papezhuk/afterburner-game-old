#pragma once

#include "standard_includes.h"
#include "genericprojectileweapon.h"

class CWeaponGrenadeLauncher : public CGenericProjectileWeapon
{
public:
	virtual const CGenericWeaponAttributes& WeaponAttributes() const override;
};
