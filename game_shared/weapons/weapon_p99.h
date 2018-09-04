#pragma once

#include "standard_includes.h"
#include "genericweapon.h"

class CWeaponP99 : public CGenericWeapon
{
public:
	virtual const CGenericWeaponAttributes& WeaponAttributes() const override;
	virtual void SecondaryAttack() override;
};
