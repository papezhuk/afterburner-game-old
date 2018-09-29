#pragma once

#include "standard_includes.h"
#include "generichitscanweapon.h"

class CWeaponRaptor : public CGenericHitscanWeapon
{
public:
	virtual const CGenericWeaponAttributes& WeaponAttributes() const override;
};
