#pragma once

#include "genericweapon.h"

class CGenericProjectileWeapon : public CGenericWeapon
{
protected:
	virtual bool SwitchFire(int index,
							const CGenericWeaponAtts_FireMode& fireMode,
							const CGenericWeaponAtts_BaseFireMechanic& mechanic) override;
	virtual void SwitchPrecache(const CGenericWeaponAtts_BaseFireMechanic& mechanic) override;

private:
	void Precache(const CGenericWeaponAtts_ProjectileFireMechanic& mechanic);
	bool ProjectileFire(int index,
						const CGenericWeaponAtts_FireMode& fireMode,
						const CGenericWeaponAtts_ProjectileFireMechanic& mechanic);
};
