#pragma once

#include "standard_includes.h"
#include "genericweapon.h"

class CWeaponGenericTest : public CGenericWeapon
{
public:
	virtual void Precache() override;
	virtual const CGenericWeaponAttributes& WeaponAttributes() const override;
};
