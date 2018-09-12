#include "genericweapon.h"
#include "studio_utils_shared.h"
#include "weaponslots.h"
#include "gamerules.h"

#ifdef CLIENT_DLL
#include "cl_dll.h"
#include "cl_entity.h"
#endif

CGenericWeapon::CGenericWeapon()
	: CBasePlayerWeapon(),
	  m_FireEvents{0},
	  m_iViewModelIndex(0),
	  m_iViewModelBody(0),
	  m_iWeaponSlot(-1),
	  m_iWeaponSlotPosition(-1)
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
	FindWeaponSlotInfo();
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
	PrecacheSounds(atts.Animations().ReloadSounds());
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
	FindWeaponSlotInfo();

	const CGenericWeaponAtts_Core& core = WeaponAttributes().Core();

	p->pszName = STRING(pev->classname);
	p->iMaxClip = core.MaxClip();
	p->iSlot = m_iWeaponSlot;
	p->iPosition = m_iWeaponSlotPosition;
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

bool CGenericWeapon::FireUsingMode(int index)
{
	if ( index < 0 || index > 1 )
	{
		return false;
	}

	const CGenericWeaponAtts_BaseFireMode* fireMode = WeaponAttributes().FireMode(index);

	if ( !fireMode )
	{
		return false;
	}

	switch ( fireMode->Id() )
	{
		case CGenericWeaponAtts_BaseFireMode::e_FireMode::Hitscan:
		{
			return HitscanFire(index, *fireMode->AsType<CGenericWeaponAtts_HitscanFireMode>());
		}

		default:
		{
			return false;
		}
	}
}

bool CGenericWeapon::HitscanFire(int index, const CGenericWeaponAtts_HitscanFireMode& fireMode)
{
	if ( index < 0 || index > 1 || fireMode.FireRate() <= 0.0f || fireMode.BulletsPerShot() < 1 )
	{
		return false;
	}

	const CGenericWeaponAttributes& atts = WeaponAttributes();

	if( m_iClip <= 0 )
	{
		if( m_fFireOnEmpty )
		{
			PlayEmptySound();
			DelayFiring(0.2f);
		}

		return false;
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
	return true;
};

void CGenericWeapon::Reload()
{
	const CGenericWeaponAttributes& atts = WeaponAttributes();
	const int maxClip = atts.Core().MaxClip();

	if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1 || m_iClip == maxClip ||
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
		if ( atts.Animations().HasSounds() )
		{
			PlaySound(atts.Animations().ReloadSounds());
		}

		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + animDuration + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 0, 1);
	}
}

void CGenericWeapon::ItemPostFrame()
{
	WeaponTick();

	if( ( m_fInReload ) && ( m_pPlayer->m_flNextAttack <= UTIL_WeaponTimeBase() ) )
	{
		// TODO: Ammo needs to be sync'd to client for this to work.
#ifndef CLIENT_DLL
		// complete the reload.
		int j = Q_min( iMaxClip() - m_iClip, m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]);

		// Add them to the clip
		m_iClip += j;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= j;
#else
	// No idea why this is so arbitrary, but Half Life had it in...
	m_iClip += 10;
#endif

		m_fInReload = FALSE;
	}

	if( !(m_pPlayer->pev->button & IN_ATTACK ) )
	{
		m_flLastFireTime = 0.0f;
	}

	if( ( m_pPlayer->pev->button & IN_ATTACK2 ) && CanAttack( m_flNextSecondaryAttack, gpGlobals->time, UseDecrement() ) )
	{
		SetFireOnEmptyState(1);
		SecondaryAttack();
	}
	else if( ( m_pPlayer->pev->button & IN_ATTACK ) && CanAttack( m_flNextPrimaryAttack, gpGlobals->time, UseDecrement() ) )
	{
		SetFireOnEmptyState(0);
		PrimaryAttack();
	}
	else if( m_pPlayer->pev->button & IN_RELOAD && iMaxClip() != WEAPON_NOCLIP && !m_fInReload )
	{
		// reload when reload is pressed, or if no buttons are down and weapon is empty.
		Reload();
	}
	else if( !( m_pPlayer->pev->button & ( IN_ATTACK | IN_ATTACK2 ) ) )
	{
		// no fire buttons down
		m_fFireOnEmpty = FALSE;

#ifndef CLIENT_DLL
		if( !IsUseable() && m_flNextPrimaryAttack < ( UseDecrement() ? 0.0 : gpGlobals->time ) )
		{
			// weapon isn't useable, switch.
			if( !( iFlags() & ITEM_FLAG_NOAUTOSWITCHEMPTY ) && g_pGameRules->GetNextBestWeapon( m_pPlayer, this ) )
			{
				m_flNextPrimaryAttack = ( UseDecrement() ? 0.0 : gpGlobals->time ) + 0.3;
				return;
			}
		}
		else
#endif
		{
			// weapon is useable. Reload if empty and weapon has waited as long as it has to after firing
			if( m_iClip == 0 && !(iFlags() & ITEM_FLAG_NOAUTORELOAD ) && m_flNextPrimaryAttack < ( UseDecrement() ? 0.0 : gpGlobals->time ) )
			{
				Reload();
				return;
			}
		}

		WeaponIdle();
		return;
	}

	// catch all
	if( ShouldWeaponIdle() )
	{
		WeaponIdle();
	}
}

void CGenericWeapon::SetFireOnEmptyState(uint8_t mode)
{
	const CGenericWeaponAtts_BaseFireMode* const fireMode = WeaponAttributes().FireMode(mode);

	if ( !fireMode )
	{
		return;
	}

	const char* ammoName = fireMode->UsesSecondaryAmmo() ? pszAmmo2() : pszAmmo1();
	int ammoIndex = fireMode->UsesSecondaryAmmo() ? SecondaryAmmoIndex() : PrimaryAmmoIndex();

	if( ammoName && ammoIndex >= 0 && m_pPlayer->m_rgAmmo[ammoIndex] < 1 )
	{
		m_fFireOnEmpty = TRUE;
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
	const uint32_t anim = idleAnims.List().ItemByProbabilisticValue(flRand);

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + ViewModelAnimationDuration(anim);
	SendWeaponAnim(anim, m_iViewModelBody);
}

void CGenericWeapon::PlaySound(const CGenericWeaponAttributes_Sound& sound, int channel)
{
	if ( sound.SoundList().Count() < 1 )
	{
		return;
	}

	const float flRand = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 0.0, 1.0);
	const char* soundName = sound.SoundList().ItemByProbabilisticValue(flRand);
	const float volume = (sound.MinVolume() < sound.MaxVolume())
		? UTIL_SharedRandomFloat(m_pPlayer->random_seed, sound.MinVolume(), sound.MaxVolume())
		: sound.MaxVolume();
	const int pitch = (sound.MinPitch() < sound.MaxPitch())
		? UTIL_SharedRandomLong(m_pPlayer->random_seed, sound.MinPitch(), sound.MaxPitch())
		: sound.MaxPitch();

	EMIT_SOUND_DYN(ENT(m_pPlayer->pev),
				   channel,
				   soundName,
				   volume,
				   sound.Attenuation(),
				   sound.Flags(),
				   pitch);
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

void CGenericWeapon::DelayPendingActions(float secs, bool allowIfEarlier)
{
	DelayFiring(secs, allowIfEarlier);
	SetNextIdleTime(secs, allowIfEarlier);
}

void CGenericWeapon::DelayFiring(float secs, bool allowIfEarlier)
{
	SetNextPrimaryAttack(secs, allowIfEarlier);
	SetNextSecondaryAttack(secs, allowIfEarlier);
}

int CGenericWeapon::iItemSlot()
{
	ASSERT(m_iWeaponSlot >= 0);
	return m_iWeaponSlot;
}

void CGenericWeapon::FindWeaponSlotInfo()
{
	if ( m_iWeaponSlot >= 0 && m_iWeaponSlotPosition >= 0 )
	{
		return;
	}

	const int id = static_cast<const int>(WeaponAttributes().Core().Id());

	for ( int slot = 0; slot < MAX_WEAPON_SLOTS; ++slot )
	{
		for ( int position = 0; position < MAX_WEAPON_POSITIONS; ++position )
		{
			if ( WEAPON_HUD_SLOTS[slot][position] == id )
			{
				m_iWeaponSlot = slot;
				m_iWeaponSlotPosition = position;
				return;
			}
		}
	}

	ASSERTSZ(false, "No slot/position found for this weapon.");
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
