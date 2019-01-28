#pragma once

#include "standard_includes.h"
#include "genericprojectileweapon.h"
#include "weapons.h"

class CWeaponGrenadeLauncher_Grenade;

class CWeaponGrenadeLauncher : public CGenericProjectileWeapon
{
public:
	virtual const CGenericWeaponAttributes& WeaponAttributes() const override;
	virtual void Precache() override;

protected:
#ifndef CLIENT_DLL
	virtual void CreateProjectile(int index,
								  const CGenericWeaponAtts_FireMode& fireMode,
								  const CGenericWeaponAtts_BaseFireMechanic& mechanic) override;

private:
	static CWeaponGrenadeLauncher_Grenade* CreateGrenade(entvars_t *pevOwner, const Vector& location, const Vector& launchDir);
#endif
};

class CWeaponGrenadeLauncher_Grenade : public CGrenade
{
public:
	virtual void Spawn() override;

#ifndef CLIENT_DLL
	void SetExplodeOnContact(bool explodeOnContact);
	void SetTumbleAngVel(float vel);
	void SetRandomTumbleAngVel(float min, float max);
	void SetDamageOnExplode(float damage);
	void SetSpeed(float speed);
	void SetFuseTime(float fuseTime);
#endif
};
