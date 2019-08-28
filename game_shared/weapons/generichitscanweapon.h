#pragma once

#include "genericweapon.h"

class CGenericWeaponAtts_HitscanFireMechanic;

class CGenericHitscanWeapon : public CGenericWeapon
{
public:
	virtual void WeaponIdle() override;

protected:
	virtual bool SwitchFire(int index,
							const CGenericWeaponAtts_FireMode& fireMode,
							const CGenericWeaponAtts_BaseFireMechanic& mechanic) override;
	virtual void SwitchPrecache(const CGenericWeaponAtts_BaseFireMechanic& mechanic) override;

private:
	void Precache(const CGenericWeaponAtts_HitscanFireMechanic& mechanic);
	bool HitscanFire(int index,
					 const CGenericWeaponAtts_FireMode& fireMode,
					 const CGenericWeaponAtts_HitscanFireMechanic& mechanic);
	Vector FireBulletsPlayer(const CGenericWeaponAtts_FireMode& fireMode,
							 const CGenericWeaponAtts_HitscanFireMechanic& mechanic,
							 const Vector& vecSrc,
							 const Vector& vecDirShooting);

#ifdef CLIENT_DLL
	Vector FireBulletsPlayer_Client(const CGenericWeaponAtts_FireMode& fireMode,
									const CGenericWeaponAtts_HitscanFireMechanic& mechanic);
#endif
};
