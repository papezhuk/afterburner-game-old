#pragma once

#include "standard_includes.h"
#include "generichitscanweapon.h"

class CWeaponGenericTest : public CGenericHitscanWeapon
{
public:
	virtual void Precache() override;
	virtual const CGenericWeaponAttributes& WeaponAttributes() const override;
};
