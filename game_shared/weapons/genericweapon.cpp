#include "genericweapon.h"
#include "studio_utils_shared.h"
#include "weaponslots.h"
#include "gamerules.h"

#ifdef CLIENT_DLL
#include "cl_dll.h"
#include "cl_entity.h"
#endif

namespace
{
	static constexpr const char* DEFAULT_WEAPON_PICKUP_SOUND = "items/gunpickup1.wav";
}

CGenericWeapon::CGenericWeapon()
	: CBasePlayerWeapon(),
	  m_FireEvents{0},
	  m_iViewModelIndex(0),
	  m_iViewModelBody(0),
	  m_iWeaponSlot(-1),
	  m_iWeaponSlotPosition(-1),
	  m_bPrimaryAttackHeldDown(false),
	  m_bSecondaryAttackHeldDown(false)
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

	if ( core.WorldModelName() )
	{
		SET_MODEL(ENT(pev), core.WorldModelName());
	}

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
	const CGenericWeaponAtts_FireMode& fireMode = atts.FireMode(fireModeIndex);

	if ( !fireMode.Event() || !fireMode.HasMechanic() )
	{
		m_FireEvents[fireModeIndex] = 0;
		return;
	}

	m_FireEvents[fireModeIndex] = PRECACHE_EVENT(1, fireMode.Event());

	SwitchPrecache(*fireMode.Mechanic());
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
	if ( core.ViewModelName() )
	{
		m_iViewModelIndex = PRECACHE_MODEL(core.ViewModelName());
		StudioGetAnimationDurations(m_iViewModelIndex, m_ViewAnimDurations);
	}

	if ( core.PlayerModelName() )
	{
		PRECACHE_MODEL(core.PlayerModelName());
	}

	if ( core.WorldModelName() )
	{
		PRECACHE_MODEL(core.WorldModelName());
	}

	const char* pickupSoundOverride = core.PickupSoundOverride();
	PRECACHE_SOUND(pickupSoundOverride ? pickupSoundOverride : DEFAULT_WEAPON_PICKUP_SOUND);
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
	const CGenericWeaponAtts_Animations& anims = WeaponAttributes().Animations();

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
	if ( index < 0 || index >= WEAPON_MAX_FIRE_MODES )
	{
		return false;
	}

	const CGenericWeaponAtts_FireMode& fireMode = WeaponAttributes().FireMode(index);

	if ( !fireMode.HasMechanic() )
	{
		return false;
	}

	if ( (m_pPlayer->pev->waterlevel == 3 && !fireMode.FiresUnderwater()) || !HasAmmo(fireMode) )
	{
		if( m_fFireOnEmpty )
		{
			PlayEmptySound();
			DelayFiring(0.2f, false, index);
		}

		return false;
	}

	return SwitchFire(index, fireMode, *fireMode.Mechanic());
}

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
		m_bPrimaryAttackHeldDown = false;
	}

	if ( !(m_pPlayer->pev->button & IN_ATTACK2) )
	{
		m_bSecondaryAttackHeldDown = false;
	}

	if( (m_pPlayer->pev->button & IN_ATTACK2) &&
		CanAttack(m_flNextSecondaryAttack, gpGlobals->time, UseDecrement()) &&
		(WeaponAttributes().FireMode(1).FullAuto() || !m_bSecondaryAttackHeldDown) )
	{
		SetFireOnEmptyState(1);
		SecondaryAttack();
		m_bSecondaryAttackHeldDown = true;
	}
	else if( (m_pPlayer->pev->button & IN_ATTACK) &&
			 CanAttack(m_flNextPrimaryAttack, gpGlobals->time, UseDecrement()) &&
			 (WeaponAttributes().FireMode(0).FullAuto() || !m_bPrimaryAttackHeldDown) )
	{
		SetFireOnEmptyState(0);
		PrimaryAttack();
		m_bPrimaryAttackHeldDown = true;
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
	const CGenericWeaponAtts_FireMode& fireMode = WeaponAttributes().FireMode(mode);

	if ( !fireMode.HasMechanic() || fireMode.UsesAmmo() == CGenericWeaponAtts_FireMode::AmmoType_e::None )
	{
		return;
	}

	const char* ammoName = fireMode.UsesAmmo() == CGenericWeaponAtts_FireMode::AmmoType_e::Primary ? pszAmmo1() : pszAmmo2();
	int ammoIndex = fireMode.UsesAmmo() == CGenericWeaponAtts_FireMode::AmmoType_e::Primary ? PrimaryAmmoIndex() : SecondaryAmmoIndex();

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
	if ( anim < 0 || anim >= m_ViewAnimDurations.Count() )
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

void CGenericWeapon::DelayFiring(float secs, bool allowIfEarlier, int mode)
{
	if ( mode < 0 || mode == 0 )
	{
		SetNextPrimaryAttack(secs, allowIfEarlier);
	}

	if ( mode < 0 || mode == 1 )
	{
		SetNextSecondaryAttack(secs, allowIfEarlier);
	}
}

bool CGenericWeapon::HasAmmo(const CGenericWeaponAtts_FireMode& fireMode, int minCount, bool useClip) const
{
	switch ( fireMode.UsesAmmo() )
	{
		case CGenericWeaponAtts_FireMode::AmmoType_e::Primary:
		{
			return useClip
				? (m_iClip >= minCount)
				: (m_iPrimaryAmmoType >= 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= minCount);
		}

		case CGenericWeaponAtts_FireMode::AmmoType_e::Secondary:
		{
			return m_iSecondaryAmmoType >= 0 && m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] >= minCount;
		}

		default:
		{
			// Treat as an infinite pool.
			return true;
		}
	}
}

bool CGenericWeapon::DecrementAmmo(const CGenericWeaponAtts_FireMode& fireMode, int decrement)
{
	switch ( fireMode.UsesAmmo() )
	{
		case CGenericWeaponAtts_FireMode::AmmoType_e::Primary:
		{
			if ( m_iClip < decrement )
			{
				return false;
			}

			m_iClip -= decrement;
			return true;
		}

		case CGenericWeaponAtts_FireMode::AmmoType_e::Secondary:
		{
			if ( m_iSecondaryAmmoType < 0 || m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] < decrement )
			{
				return false;
			}

			m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] -= decrement;
			return true;
		}

		default:
		{
			// Treat as an infinite pool.
			return true;
		}
	}
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
