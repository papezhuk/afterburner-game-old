#include "genericweapon.h"

CGenericWeapon::~CGenericWeapon()
{
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
	PrecacheCore(WeaponAttributes().Core());
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
		m_FireEvents[fireModeIndex] = PRECACHE_EVENT(1, atts.FireMode(0)->EventName());
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
	if ( fireMode.ShellModelName() )
	{
		PRECACHE_MODEL(fireMode.ShellModelName());
	}

	PrecacheSounds(fireMode.Sounds());
}

void CGenericWeapon::PrecacheSounds(const CGenericWeaponAttributes_Sound& sounds)
{
	const WeightedValueList<const char*>& soundList = sounds.SoundList();

	for ( uint32_t index = 0; index < soundList.Count(); ++index )
	{
		const char* const soundName = soundList.Value(index);
		if ( soundName )
		{
			PRECACHE_SOUND(soundName);
		}
	}
}

void CGenericWeapon::PrecacheCore(const CGenericWeaponAtts_Core& core)
{
	PRECACHE_MODEL(core.ViewModelName());
	PRECACHE_MODEL(core.PlayerModelName());
	PRECACHE_MODEL(core.WorldModelName());
}

int CGenericWeapon::GetItemInfo(ItemInfo *p)
{
	const CGenericWeaponAtts_Core& core = WeaponAttributes().Core();

	p->pszName = STRING(core.Classname());
	p->pszAmmo1 = core.PrimaryAmmoName();
	p->iMaxAmmo1 = core.PrimaryAmmoMax();
	p->pszAmmo2 = core.SecondaryAmmoName();
	p->iMaxAmmo2 = core.SecondaryAmmoMax();
	p->iMaxClip = core.MaxClip();
	p->iSlot = core.WeaponSlot();
	p->iPosition = core.WeaponSlotPosition();
	p->iFlags = core.Flags();
	p->iId = m_iId = static_cast<int>(core.Id());
	p->iWeight = core.SwitchWeight();

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

	vecDir = m_pPlayer->FireBulletsPlayer(fireMode.BulletsPerShot(),
										  vecSrc,
										  vecAiming,
										  Vector(spreadX, spreadY, 0.0f),
										  8192,
										  fireMode.BulletType(),
										  0,
										  0,
										  m_pPlayer->pev,
										  m_pPlayer->random_seed);

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

int CGenericWeapon::iItemSlot()
{
	return WeaponAttributes().Core().WeaponSlot();
}
