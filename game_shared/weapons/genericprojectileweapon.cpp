#include "genericprojectileweapon.h"
#include "soundent.h"

void CGenericProjectileWeapon::SwitchPrecache(const CGenericWeaponAtts_BaseFireMechanic& mechanic)
{
	if ( mechanic.Id() != CGenericWeaponAtts_BaseFireMechanic::FireMechanic_e::Projectile )
	{
		return;
	}

	Precache(static_cast<const CGenericWeaponAtts_ProjectileFireMechanic&>(mechanic));
}

void CGenericProjectileWeapon::Precache(const CGenericWeaponAtts_ProjectileFireMechanic& mechanic)
{
	// Nothing at the moment.
}

bool CGenericProjectileWeapon::SwitchFire(int index,
									   const CGenericWeaponAtts_FireMode& fireMode,
									   const CGenericWeaponAtts_BaseFireMechanic& mechanic)
{
	if ( mechanic.Id() != CGenericWeaponAtts_BaseFireMechanic::FireMechanic_e::Projectile )
	{
		return false;
	}

	return ProjectileFire(index, fireMode, static_cast<const CGenericWeaponAtts_ProjectileFireMechanic&>(mechanic));
}

bool CGenericProjectileWeapon::ProjectileFire(int index, const CGenericWeaponAtts_FireMode& fireMode, const CGenericWeaponAtts_ProjectileFireMechanic& mechanic)
{
	if ( index < 0 || index > 1 || fireMode.FireRate() <= 0.0f )
	{
		return false;
	}

	m_pPlayer->m_iWeaponVolume = fireMode.Volume();
	m_pPlayer->m_iWeaponFlash = fireMode.MuzzleFlashBrightness();

	m_pPlayer->m_iExtraSoundTypes = bits_SOUND_DANGER;
	m_pPlayer->m_flStopExtraSoundTime = UTIL_WeaponTimeBase() + 0.2;

	DecrementAmmo(fireMode, 1);

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

#ifndef CLIENT_DLL
	// Only create the actual projectile on the server.
	CreateProjectile(index, fireMode, mechanic);
#endif

	PLAYBACK_EVENT(DefaultEventFlags(), m_pPlayer->edict(), m_FireEvents[index]);

	DelayFiring(1.0f / fireMode.FireRate());
	SetNextIdleTime(5, true);

	if ( !HasAmmo(fireMode, 1, true) && !HasAmmo(fireMode, 1, false) )
	{
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
	}

	return true;
}
