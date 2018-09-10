#include "genericweapon.h"
#include "studio_utils_shared.h"

#ifdef CLIENT_DLL
#include "cl_dll.h"
#include "cl_entity.h"
#endif

CGenericWeapon::CGenericWeapon()
	: CBasePlayerWeapon(),
	  m_iViewModelIndex(0),
	  m_iViewModelBody(0)
{
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
	PRECACHE_MODEL(fireMode.ShellModelName());
	PrecacheSounds(fireMode.Sounds());
}

void CGenericWeapon::PrecacheSounds(const CGenericWeaponAttributes_Sound& sounds)
{
	const WeightedValueList<const char*>& soundList = sounds.SoundList();

	for ( uint32_t index = 0; index < soundList.Count(); ++index )
	{
		PRECACHE_SOUND(soundList.Value(index));
	}
}

void CGenericWeapon::PrecacheCore(const CGenericWeaponAtts_Core& core)
{
	m_iViewModelIndex = PRECACHE_MODEL(core.ViewModelName());
	StudioGetAnimationDurations(m_iViewModelIndex, m_ViewAnimDurations);

	PRECACHE_MODEL(core.PlayerModelName());
	PRECACHE_MODEL(core.WorldModelName());
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

	return DefaultDeploy(core.ViewModelName(), core.PlayerModelName(), anims.Index_Draw(), anims.Extension(), m_iViewModelBody);
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
			SetNextPrimaryAttack(0.2f);
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
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

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

	vecDir = FireBulletsPlayer(fireMode, vecSrc, vecAiming);

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
							m_pPlayer->random_seed,
							0,
							m_iClip == 0 ? 1 : 0,
							0);
	}

	DelayFiring(1.0f / fireMode.FireRate());

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

	if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == maxClip ||
		 m_flNextPrimaryAttack > UTIL_WeaponTimeBase() )
	{
		return;
	}

	if ( atts.Core().UsesSpecialReload() )
	{
		// Reload is more complicated, so let the derived weapon class do it.
		m_fInSpecialReload = HandleSpecialReload(m_fInSpecialReload);
		return;
	}

	int anim = m_iClip < 1
		? atts.Animations().Index_ReloadWhenEmpty()
		: atts.Animations().Index_ReloadWhenNotEmpty();

	if ( m_iClip < 1 && anim < 0 )
	{
		anim = atts.Animations().Index_ReloadWhenNotEmpty();
	}

	const float animDuration = ViewModelAnimationDuration(anim);

	if ( DefaultReload(maxClip, anim, animDuration, m_iViewModelBody) )
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + animDuration + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 0, 1);
	}
}

int CGenericWeapon::HandleSpecialReload(int currentState)
{
	return 0;
}

void CGenericWeapon::WeaponIdle()
{
	const CGenericWeaponAttributes& atts = WeaponAttributes();
	const CGenericWeaponAtts_Core& core = atts.Core();

	ResetEmptySound();

	if ( atts.FireMode(0) && atts.FireMode(0)->Id() == CGenericWeaponAtts_BaseFireMode::e_FireMode::Hitscan )
	{
		m_pPlayer->GetAutoaimVector(atts.FireMode(0)->AsType<CGenericWeaponAtts_HitscanFireMode>()->AutoAim());
	}

	if( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
	{
		return;
	}

	if ( core.UsesSpecialReload() ? IdleProcess_CheckSpecialReload() : IdleProcess_CheckReload() )
	{
		// Reload did something, so don't play idle animations;
		return;
	}

	IdleProcess_PlayIdleAnimation();
}

bool CGenericWeapon::IdleProcess_CheckReload()
{
	if ( WeaponAttributes().Core().AutoReload() && m_iClip < 1 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0 )
	{
		Reload();
		return true;
	}

	return false;
}

bool CGenericWeapon::IdleProcess_CheckSpecialReload()
{
	if ( m_fInSpecialReload > 0 ||									// Haven't finished reloading yet
		 (m_iClip < 1 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]) )	// Need to fire off reload sequence automatically
	{
		m_fInSpecialReload = HandleSpecialReload(m_fInSpecialReload);
		return true;
	}

	return false;
}

void CGenericWeapon::IdleProcess_PlayIdleAnimation()
{
	const CGenericWeaponAtts_IdleAnimations& idleAnims = WeaponAttributes().IdleAnimations();

	if ( idleAnims.List().Count() < 1 || (!idleAnims.IdleWhenClipEmpty() && m_iClip < 1) )
	{
		return;
	}

	const float flRand = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 0.0, 1.0);
	const uint32_t anim = idleAnims.List().IndexByProbabilisticValue(flRand);

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + ViewModelAnimationDuration(anim);
	SendWeaponAnim(anim, m_iViewModelBody);
}

Vector CGenericWeapon::FireBulletsPlayer(const CGenericWeaponAtts_HitscanFireMode& fireMode,
										 const Vector& vecSrc,
										 const Vector& vecDirShooting)
{
#ifdef CLIENT_DLL
	// The client doesn't actually do any bullet simulation, we just make sure that
	// the generated random vectors match up.
	return FireBulletsPlayer_Client(fireMode);
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

	const uint32_t numShots = fireMode.BulletsPerShot();
	for( uint32_t shot = 1; shot <= numShots; shot++ )
	{
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

			pEntity->TraceAttack(pevAttacker, fireMode.DamagePerShot(), vecDir, &tr, DMG_BULLET);
			TEXTURETYPE_PlaySound(&tr, vecSrc, vecEnd, BULLET_GENERIC);
		}

		// make bullet trails
		UTIL_BubbleTrail(vecSrc, tr.vecEndPos, (int)((DEFAULT_BULLET_TRACE_DISTANCE * tr.flFraction) / 64.0));
	}

	ApplyMultiDamage(pev, pevAttacker);

	return Vector(x * fireMode.SpreadX(), y * fireMode.SpreadY(), 0.0);
#endif
}

void CGenericWeapon::SetViewModelBody(int body, bool immediate)
{
	m_iViewModelBody = body;

#ifdef CLIENT_DLL
	if ( immediate )
	{
		struct cl_entity_s* viewModel = gEngfuncs.GetViewModel();
		if ( viewModel )
		{
			viewModel->curstate.body = body;
		}
	}
#endif
};

float CGenericWeapon::ViewModelAnimationDuration(int anim) const
{
	if ( anim < 0 || anim >= m_ViewAnimDurations.size() )
	{
		return 0.0f;
	}

	return m_ViewAnimDurations[anim];
}

void CGenericWeapon::DelayPendingActions(float secs)
{
	DelayFiring(secs);
	SetNextIdleTime(secs);
}

void CGenericWeapon::DelayFiring(float secs)
{
	SetNextPrimaryAttack(secs);
	SetNextSecondaryAttack(secs);
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
Vector CGenericWeapon::FireBulletsPlayer_Client(const CGenericWeaponAtts_HitscanFireMode& fireMode)
{
	float x = 0, y = 0;

	const uint32_t numShots = fireMode.BulletsPerShot();
	for( uint32_t shot = 1; shot <= numShots; shot++ )
	{
		GetSharedCircularGaussianSpread(shot, m_pPlayer->random_seed, x, y);
	}

	return Vector(x * fireMode.SpreadX(), y * fireMode.SpreadY(), 0.0);
}
#endif
