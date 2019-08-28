#pragma once

#include "standard_includes.h"
#include "genericweapon.h"

class CWeaponFists : public CGenericWeapon
{
public:
	CWeaponFists();
	virtual const CGenericWeaponAttributes& WeaponAttributes() const override;

protected:
	virtual bool SwitchFire(int index,
							const CGenericWeaponAtts_FireMode& fireMode,
							const CGenericWeaponAtts_BaseFireMechanic& mechanic) override;
	virtual void SwitchPrecache(const CGenericWeaponAtts_BaseFireMechanic& mechanic) override;
};