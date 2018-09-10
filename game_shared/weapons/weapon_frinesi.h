#pragma once

#include "standard_includes.h"
#include "genericweapon.h"

class CWeaponFrinesi : public CGenericWeapon
{
public:
	virtual const CGenericWeaponAttributes& WeaponAttributes() const override;
};
