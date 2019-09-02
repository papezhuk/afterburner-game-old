#pragma once

#include "standard_includes.h"
#include "genericprojectileweapon.h"
#include "weapons.h"

class CWeaponGrenadeLauncher_Grenade;

class CWeaponGrenadeLauncher : public CGenericProjectileWeapon
{
public:
	virtual const WeaponAtts::WACollection& WeaponAttributes() const override;
	virtual void Precache() override;

#ifndef CLIENT_DLL
	virtual float Bot_CalcDesireToUse(CGenericWeapon& weapon, CBaseBot& bot, CBaseEntity& enemy, float distanceToEnemy) const override;
	virtual void Bot_SetFightStyle(CBaseBotFightStyle& fightStyle) const override;
#endif

protected:
#ifndef CLIENT_DLL
	virtual void CreateProjectile(int index,
								  const CGenericWeaponAtts_FireMode& fireMode,
								  const CGenericWeaponAtts_BaseFireMechanic& mechanic) override;

private:
	static CWeaponGrenadeLauncher_Grenade* CreateGrenade(entvars_t *pevOwner, const Vector& location, const Vector& launchDir);

	Vector GetGrenadeLaunchAngles() const;
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
