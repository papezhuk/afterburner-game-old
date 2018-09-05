#include "genericweapon.h"

namespace
{
	constexpr float BULLET_TRACE_DISTANCE = 8192;

	inline void PrecacheSoundSafe(const char* sound)
	{
		if ( sound )
		{
			PRECACHE_SOUND(sound);
		}
	}

	inline void PrecacheModelSafe(const char* modelName)
	{
		if ( modelName )
		{
			PRECACHE_MODEL(modelName);
		}
	}
}

CGenericWeapon::~CGenericWeapon()
{
	// Default implementation of blank virtual destructor.
}

void CGenericWeapon::Spawn()
{
	const CGenericWeaponAtts_Core& core = WeaponAttributes().Core();
	core.Validate();

	pev->classname = MAKE_STRING(core.Classname()); // hack to allow for old names
	Precache();
	m_iId = static_cast<int>(core.Id());
	SET_MODEL(ENT(pev), core.WorldModelName());

	m_iDefaultAmmo = core.PrimaryAmmoOnFirstPickup();

	FallInit();// get ready to fall down.
}

void CGenericWeapon::Precache()
{
	const CGenericWeaponAttributes& atts = WeaponAttributes();

	PrecacheCore(atts.Core());
	PrecacheFireMode(0);
	PrecacheFireMode(1);
}

void CGenericWeapon::PrecacheFireMode(uint8_t fireModeIndex)
{
	const CGenericWeaponAttributes& atts = WeaponAttributes();
	const CGenericWeaponAtts_BaseFireMode* fireMode = atts.FireMode(fireModeIndex);

	if ( !fireMode || !fireMode->EventName() )
	{
		m_FireEvents[fireModeIndex] = 0;
	}
	else
	{
		m_FireEvents[fireModeIndex] = PRECACHE_EVENT(1, fireMode->EventName());
	}

	switch ( fireMode->Id() )
	{
		case CGenericWeaponAtts_BaseFireMode::e_FireMode::Hitscan:
		{
			PrecacheHitscanResources(*fireMode->AsType<const CGenericWeaponAtts_HitscanFireMode>());
			break;
		}

		default:
		{
			break;
		}
	}
}

void CGenericWeapon::PrecacheHitscanResources(const CGenericWeaponAtts_HitscanFireMode& fireMode)
{
	PrecacheModelSafe(fireMode.ShellModelName());
	PrecacheSounds(fireMode.Sounds());
}

void CGenericWeapon::PrecacheSounds(const CGenericWeaponAttributes_Sound& sounds)
{
	const WeightedValueList<const char*>& soundList = sounds.SoundList();

	for ( uint32_t index = 0; index < soundList.Count(); ++index )
	{
		PrecacheSoundSafe(soundList.Value(index));
	}
}

void CGenericWeapon::PrecacheCore(const CGenericWeaponAtts_Core& core)
{
	PrecacheModelSafe(core.ViewModelName());
	PrecacheModelSafe(core.PlayerModelName());
	PrecacheModelSafe(core.WorldModelName());
}

int CGenericWeapon::GetItemInfo(ItemInfo *p)
{
	const CGenericWeaponAtts_Core& core = WeaponAttributes().Core();

	p->pszName = STRING( pev->classname );
	p->iMaxClip = core.MaxClip();
	p->iSlot = core.WeaponSlot();
	p->iPosition = core.WeaponSlotPosition();
	p->iFlags = core.Flags();
	p->iId = m_iId = static_cast<int>(core.Id());
	p->iWeight = core.SwitchWeight();

	const CAmmoDef* primaryAmmo = core.PrimaryAmmoDef();
	if ( primaryAmmo )
	{
		p->pszAmmo1 = primaryAmmo->Name;
		p->iMaxAmmo1 = primaryAmmo->MaxCarry;
	}
	else
	{
		p->pszAmmo1 = NULL;
		p->iMaxAmmo1 = -1;
	}

	const CAmmoDef* secondaryAmmo = core.SecondaryAmmoDef();
	if ( secondaryAmmo )
	{
		p->pszAmmo2 = secondaryAmmo->Name;
		p->iMaxAmmo2 = secondaryAmmo->MaxCarry;
	}
	else
	{
		p->pszAmmo2 = NULL;
		p->iMaxAmmo2 = -1;
	}

	return 1;
}

int CGenericWeapon::AddToPlayer(CBasePlayer *pPlayer)
{
	if( !CBasePlayerWeapon::AddToPlayer(pPlayer) )
	{
		return FALSE;
	}

	MESSAGE_BEGIN(MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev);
		WRITE_BYTE(m_iId);
	MESSAGE_END();
	return TRUE;
}

BOOL CGenericWeapon::Deploy()
{
	const CGenericWeaponAtts_Core& core = WeaponAttributes().Core();
	const CGenericWeaponAtts_Animations anims = WeaponAttributes().Animations();

	return DefaultDeploy(core.ViewModelName(), core.PlayerModelName(), anims.Index_Draw(), anims.Extension());
}

void CGenericWeapon::PrimaryAttack()
{
	FireUsingMode(0);
}

void CGenericWeapon::SecondaryAttack()
{
	FireUsingMode(1);
}

void CGenericWeapon::FireUsingMode(int index)
{
	if ( index < 0 || index > 1 )
	{
		return;
	}

	const CGenericWeaponAtts_BaseFireMode* fireMode = WeaponAttributes().FireMode(index);

	if ( !fireMode )
	{
		return;
	}

	switch ( fireMode->Id() )
	{
		case CGenericWeaponAtts_BaseFireMode::e_FireMode::Hitscan:
		{
			HitscanFire(index, *fireMode->AsType<CGenericWeaponAtts_HitscanFireMode>());
			return;
		}

		default:
		{
			return;
		}
	}
}

void CGenericWeapon::HitscanFire(int index, const CGenericWeaponAtts_HitscanFireMode& fireMode)
{
	if ( index < 0 || index > 1 || fireMode.FireRate() <= 0.0f || fireMode.BulletsPerShot() < 1 )
	{
		return;
	}

	const CGenericWeaponAttributes& atts = WeaponAttributes();

	if( m_iClip <= 0 )
	{
		if( m_fFireOnEmpty )
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = GetNextAttackDelay( 0.2 );
		}

		return;
	}

	m_iClip--;

	m_pPlayer->pev->effects = (int)( m_pPlayer->pev->effects ) | EF_MUZZLEFLASH;

	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif
	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	m_pPlayer->m_iWeaponVolume = fireMode.Volume();
	m_pPlayer->m_iWeaponFlash = fireMode.MuzzleFlashBrightness();

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming;

	if( fireMode.AutoAim() > 0.0f )
	{
		vecAiming = m_pPlayer->GetAutoaimVector(fireMode.AutoAim());
	}
	else
	{
		vecAiming = gpGlobals->v_forward;
	}

	Vector vecDir;
	const float spreadX = fireMode.SpreadX();
	const float spreadY = fireMode.SpreadY();

	vecDir = FireBulletsPlayer(fireMode, 1, vecSrc, vecAiming);

	if ( m_FireEvents[index] )
	{
		PLAYBACK_EVENT_FULL(flags,
							m_pPlayer->edict(),
							m_FireEvents[index],
							0.0,
							(float *)&g_vecZero,
							(float *)&g_vecZero,
							vecDir.x,
							vecDir.y,
							static_cast<int>(atts.Core().Id()),
							index,
							m_iClip == 0 ? 1 : 0,
							0);
	}

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = GetNextAttackDelay(1.0f / fireMode.FireRate());

	if( !m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
	{
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );
	}

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
};

void CGenericWeapon::Reload()
{
	const CGenericWeaponAttributes& atts = WeaponAttributes();
	const int maxClip = atts.Core().MaxClip();

	if( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == maxClip )
	{
		return;
	}

	int iResult;

	if( m_iClip == 0 )
	{
		iResult = DefaultReload( maxClip, atts.Animations().Index_ReloadWhenEmpty(), 1.5 );
	}
	else
	{
		iResult = DefaultReload( maxClip, atts.Animations().Index_ReloadWhenNotEmpty(), 1.5 );
	}

	if( iResult )
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
	}
}

void CGenericWeapon::WeaponIdle()
{
	const CGenericWeaponAttributes& atts = WeaponAttributes();
	const CGenericWeaponAtts_IdleAnimations& idleAnims = atts.IdleAnimations();

	ResetEmptySound();

	if ( atts.FireMode(0) && atts.FireMode(0)->Id() == CGenericWeaponAtts_BaseFireMode::e_FireMode::Hitscan )
	{
		m_pPlayer->GetAutoaimVector(atts.FireMode(0)->AsType<CGenericWeaponAtts_HitscanFireMode>()->AutoAim());
	}

	if( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
	{
		return;
	}

	// only idle if the slide isn't back
	if( m_iClip != 0 && idleAnims.List().Count() > 0 )
	{
		// TODO: Placeholder value here. Calculate automatically, or allow user to specify.
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 60.0 / 16.0;

		const float flRand = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 0.0, 1.0);
		SendWeaponAnim(idleAnims.List().IndexByProbabilisticValue(flRand));
	}
}

Vector CGenericWeapon::FireBulletsPlayer(const CGenericWeaponAtts_HitscanFireMode& fireMode,
										 uint32_t numShots,
										 const Vector& vecSrc,
										 const Vector& vecDirShooting)
{
#ifdef CLIENT_DLL
	// The client doesn't actually do any bullet simulation, we just make sure that
	// the generated random vectors match up.
	return FireBulletsPlayer_Client(fireMode, numShots);
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

	for( uint32_t shot = 1; shot <= numShots; shot++ )
	{
		GetSharedCircularGaussianSpread(shot, shared_rand, x, y);

		Vector vecDir = vecDirShooting +
						x * fireMode.SpreadX() * vecRight +
						y * fireMode.SpreadY() * vecUp;
		Vector vecEnd;

		vecEnd = vecSrc + vecDir * BULLET_TRACE_DISTANCE;
		UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(pev), &tr);

		// do damage, paint decals
		if( tr.flFraction != 1.0 )
		{
			CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

			pEntity->TraceAttack(pevAttacker, fireMode.DamagePerShot(), vecDir, &tr, DMG_BULLET);
			TEXTURETYPE_PlaySound(&tr, vecSrc, vecEnd, BULLET_GENERIC);
		}

		// make bullet trails
		UTIL_BubbleTrail(vecSrc, tr.vecEndPos, (int)((BULLET_TRACE_DISTANCE * tr.flFraction) / 64.0));
	}

	ApplyMultiDamage(pev, pevAttacker);

	return Vector(x * fireMode.SpreadX(), y * fireMode.SpreadY(), 0.0);
#endif
}

int CGenericWeapon::iItemSlot()
{
	return WeaponAttributes().Core().WeaponSlot();
}

void CGenericWeapon::GetSharedCircularGaussianSpread(uint32_t shot, int shared_rand, float& x, float& y)
{
	x = UTIL_SharedRandomFloat( shared_rand + shot, -0.5, 0.5 ) + UTIL_SharedRandomFloat( shared_rand + ( 1 + shot ) , -0.5, 0.5 );
	y = UTIL_SharedRandomFloat( shared_rand + ( 2 + shot ), -0.5, 0.5 ) + UTIL_SharedRandomFloat( shared_rand + ( 3 + shot ), -0.5, 0.5 );
}

#ifdef CLIENT_DLL
Vector CGenericWeapon::FireBulletsPlayer_Client(const CGenericWeaponAtts_HitscanFireMode& fireMode, uint32_t numShots)
{
	float x = 0, y = 0;

	for( uint32_t shot = 1; shot <= numShots; shot++ )
	{
		GetSharedCircularGaussianSpread(shot, m_pPlayer->random_seed, x, y);
	}

	return Vector(x * fireMode.SpreadX(), y * fireMode.SpreadY(), 0.0);
}
#endif
