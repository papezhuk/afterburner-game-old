#pragma once

#include "standard_includes.h"
#include "genericweapon.h"

class CWeaponP99 : public CGenericWeapon
{
public:
	CWeaponP99();
	virtual const CGenericWeaponAttributes& WeaponAttributes() const override;
	virtual void PrimaryAttack() override;
	virtual void SecondaryAttack() override;

private:
	bool m_bSilenced;
};
