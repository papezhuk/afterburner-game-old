#include "generichitscanweapon.h"
#include "weaponatts_hitscanfiremechanic.h"
#include "skill.h"

void CGenericHitscanWeapon::WeaponIdle()
{
	const CGenericWeaponAtts_BaseFireMechanic* const primaryMechanic = WeaponAttributes().FireMode(0).Mechanic();

	if ( primaryMechanic && primaryMechanic->Id() == CGenericWeaponAtts_BaseFireMechanic::FireMechanic_e::Hitscan )
	{
		m_pPlayer->GetAutoaimVector(primaryMechanic->AsType<CGenericWeaponAtts_HitscanFireMechanic>()->AutoAim());
	}

	CGenericWeapon::WeaponIdle();
}

void CGenericHitscanWeapon::SwitchPrecache(const CGenericWeaponAtts_BaseFireMechanic& mechanic)
{
	if ( mechanic.Id() != CGenericWeaponAtts_BaseFireMechanic::FireMechanic_e::Hitscan )
	{
		return;
	}

	Precache(static_cast<const CGenericWeaponAtts_HitscanFireMechanic&>(mechanic));
}

void CGenericHitscanWeapon::Precache(const CGenericWeaponAtts_HitscanFireMechanic& mechanic)
{
	PRECACHE_MODEL(mechanic.ShellModelName());
}

bool CGenericHitscanWeapon::SwitchFire(int index,
									   const CGenericWeaponAtts_FireMode& fireMode,
									   const CGenericWeaponAtts_BaseFireMechanic& mechanic)
{
	if ( mechanic.Id() != CGenericWeaponAtts_BaseFireMechanic::FireMechanic_e::Hitscan )
	{
		return false;
	}

	return HitscanFire(index, fireMode, static_cast<const CGenericWeaponAtts_HitscanFireMechanic&>(mechanic));
}

bool CGenericHitscanWeapon::HitscanFire(int index,
										const CGenericWeaponAtts_FireMode& fireMode,
										const CGenericWeaponAtts_HitscanFireMechanic& mechanic)
{
	if ( index < 0 || index > 1 || fireMode.FireRate() <= 0.0f || mechanic.BulletsPerShot() < 1 )
	{
		return false;
	}

	DecrementAmmo(fireMode, 1);

	m_pPlayer->pev->effects = (int)( m_pPlayer->pev->effects ) | EF_MUZZLEFLASH;

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	m_pPlayer->m_iWeaponVolume = fireMode.Volume();
	m_pPlayer->m_iWeaponFlash = fireMode.MuzzleFlashBrightness();

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming;

	if( mechanic.AutoAim() > 0.0f )
	{
		vecAiming = m_pPlayer->GetAutoaimVector(mechanic.AutoAim());
	}
	else
	{
		vecAiming = gpGlobals->v_forward;
	}

	Vector vecDir;
	const float spreadX = fireMode.SpreadX();
	const float spreadY = fireMode.SpreadY();

	vecDir = FireBulletsPlayer(fireMode, mechanic, vecSrc, vecAiming);

	if ( m_FireEvents[index] )
	{
		PLAYBACK_EVENT_FULL(DefaultEventFlags(),
							m_pPlayer->edict(),
							m_FireEvents[index],
							0.0,
							(float *)&g_vecZero,
							(float *)&g_vecZero,
							vecDir.x,
							vecDir.y,
							m_pPlayer->random_seed,
							0,
							m_iClip == 0 ? 1 : 0,
							0);
	}

	DelayFiring(1.0f / fireMode.FireRate());

	if ( !HasAmmo(fireMode, 1, true) && !HasAmmo(fireMode, 1, false) )
	{
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
	}

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
	return true;
}

Vector CGenericHitscanWeapon::FireBulletsPlayer(const CGenericWeaponAtts_FireMode& fireMode,
												const CGenericWeaponAtts_HitscanFireMechanic& mechanic,
												const Vector& vecSrc,
										 		const Vector& vecDirShooting)
{
#ifdef CLIENT_DLL
	// The client doesn't actually do any bullet simulation, we just make sure that
	// the generated random vectors match up.
	return FireBulletsPlayer_Client(fireMode, mechanic);
#else
	TraceResult tr;
	Vector vecRight = gpGlobals->v_right;
	Vector vecUp = gpGlobals->v_up;
	float x = 0.0f;
	float y = 0.0f;

	entvars_t* const pevAttacker = m_pPlayer->pev;
	const int shared_rand = m_pPlayer->random_seed;

	ClearMultiDamage();
	gMultiDamage.type = DMG_BULLET | DMG_NEVERGIB;

	const uint32_t numShots = mechanic.BulletsPerShot();
	for( uint32_t shot = 1; shot <= numShots; shot++ )
	{
		float damagePerShot = 1.0f;
		const CGenericWeaponAtts_HitscanFireMechanic::SkillBasedDamagePtr dmgPtr = mechanic.BaseDamagePerShot();
		if ( dmgPtr )
		{
			damagePerShot = gSkillData.*dmgPtr;
		}

		GetSharedCircularGaussianSpread(shot, shared_rand, x, y);

		Vector vecDir = vecDirShooting +
						x * fireMode.SpreadX() * vecRight +
						y * fireMode.SpreadY() * vecUp;
		Vector vecEnd;

		vecEnd = vecSrc + vecDir * DEFAULT_BULLET_TRACE_DISTANCE;
		UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(pev), &tr);

		// do damage, paint decals
		if( tr.flFraction != 1.0 )
		{
			CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

			pEntity->TraceAttack(pevAttacker, damagePerShot, vecDir, &tr, DMG_BULLET);
			TEXTURETYPE_PlaySound(&tr, vecSrc, vecEnd, BULLET_GENERIC);
		}

		// make bullet trails
		UTIL_BubbleTrail(vecSrc, tr.vecEndPos, (int)((DEFAULT_BULLET_TRACE_DISTANCE * tr.flFraction) / 64.0));
	}

	ApplyMultiDamage(pev, pevAttacker);

	return Vector(x * fireMode.SpreadX(), y * fireMode.SpreadY(), 0.0);
#endif
}

#ifdef CLIENT_DLL
Vector CGenericHitscanWeapon::FireBulletsPlayer_Client(const CGenericWeaponAtts_FireMode& fireMode,
													   const CGenericWeaponAtts_HitscanFireMechanic& mechanic)
{
	float x = 0, y = 0;

	const uint32_t numShots = mechanic.BulletsPerShot();
	for( uint32_t shot = 1; shot <= numShots; shot++ )
	{
		GetSharedCircularGaussianSpread(shot, m_pPlayer->random_seed, x, y);
	}

	return Vector(x * fireMode.SpreadX(), y * fireMode.SpreadY(), 0.0);
}
#endif
