#include "weapon_frinesi.h"

namespace
{
	enum FrinesiAnimations_e
	{
		FRINESI_IDLE1 = 0,
		FRINESI_SHOOT,
		FRINESI_SHOOT_BIG,
		FRINESI_RELOAD,
		FRINESI_PUMP,
		FRINESI_START_RELOAD,
		FRINESI_DRAW,
		FRINESI_HOLSTER,
		FRINESI_IDLE2,
		FRINESI_IDLE3,
		FRINESI_IDLE4
	};

	enum FrinesiReloadState_e
	{
		RELOAD_IDLE = 0,
		RELOAD_LOAD_SHELL,
		RELOAD_INCREMENT_CLIP
	};

	constexpr int RELOAD_MASK = 0x3;
	constexpr int RELOAD_FLAG_INTERRUPTED = (1 << 2);
	constexpr int RELOAD_FLAG_LOADED_ONCE = (1 << 3);
	constexpr int NextReloadState(int orig, int next)
	{
		return (orig & ~(RELOAD_MASK)) | next;
	}

	// From Nightfire:
	// - 6 bullets per shot in either mode
	// - 81 damage over all 6 shots in auto mode = 13.5 per shot
	// - 159 damage over all 6 shots in pump mode = 26.5 per shot

	constexpr uint8_t FRINESI_PELLETS_PER_SHOT = 6;
	constexpr float FRINESI_AUTOAIM_DEG = AUTOAIM_5DEGREES;

	constexpr float FRINESI_BASE_DAMAGE_AUTO = 81.0f / static_cast<float>(FRINESI_PELLETS_PER_SHOT);
	constexpr float FRINESI_BASE_SPREAD_AUTO = 0.05f;
	constexpr float FRINESI_FIRE_RATE_AUTO = 4.0f;
	constexpr float FRINESI_RECOIL_AUTO = -5.0f;

	constexpr float FRINESI_BASE_DAMAGE_PUMP = 159.0f / static_cast<float>(FRINESI_PELLETS_PER_SHOT);
	constexpr float FRINESI_BASE_SPREAD_PUMP = 0.1f;
	constexpr float FRINESI_FIRE_RATE_PUMP = 1.0f;
	constexpr float FRINESI_RECOIL_PUMP = -10.0f;

#ifdef CLIENT_DLL
	static CWeaponFrinesi PredictionWeapon;
#endif
}

static const CGenericWeaponAttributes StaticWeaponAttributes = CGenericWeaponAttributes(
	CGenericWeaponAtts_Core()
	.Id(WeaponId_e::WeaponFrinesi)
	.Classname("weapon_frinesi")
	.WeaponSlot(2)			// TODO: Remove these from attributes, define slots elsewhere
	.WeaponSlotPosition(3)
	.Flags(0)
	.SwitchWeight(0)
	.PrimaryAmmoDef(&AmmoDef_Frinesi)
	.SecondaryAmmoDef(&AmmoDef_Frinesi)
	.MaxClip(8)
	.PrimaryAmmoOnFirstPickup(8)
	.UsesSpecialReload(true)
	.ViewModelName("models/weapon_frinesi/v_frinesi.mdl")
	.PlayerModelName("models/weapon_frinesi/p_frinesi.mdl")
	.WorldModelName("models/weapon_frinesi/w_frinesi.mdl")
#ifdef CLIENT_DLL
	.ClientPredictionWeapon(&PredictionWeapon)
#endif
)
.FireMode(0, &((*new CGenericWeaponAtts_HitscanFireMode("events/weapon_frinesi/fire01.sc"))
	.FireRate(FRINESI_FIRE_RATE_AUTO)
	.UniformSpread(FRINESI_BASE_SPREAD_AUTO)
	.BulletsPerShot(FRINESI_PELLETS_PER_SHOT)
	.DamagePerShot(FRINESI_BASE_DAMAGE_AUTO)
	.AutoAim(FRINESI_AUTOAIM_DEG)
	.ShellModelName("models/shell.mdl") // TODO: Nightfire has a shell model - use that? Multiple skins?
	.AnimIndex_FireNotEmpty(FRINESI_SHOOT)
	.ViewPunchY(FRINESI_RECOIL_AUTO)
	.Volume(LOUD_GUN_VOLUME)
	.MuzzleFlashBrightness(NORMAL_GUN_FLASH)
	.Sounds(CGenericWeaponAttributes_Sound()
		.Sound("weapons/weapon_frinesi/frinesi_fire.wav")
		.MinVolume(0.95f)
		.MaxVolume(1.0f)
		.MinPitch(98)
		.MaxPitch(102)
	)
))
.FireMode(1, &((*new CGenericWeaponAtts_HitscanFireMode("events/weapon_frinesi/fire02.sc"))
	.FireRate(FRINESI_FIRE_RATE_PUMP)
	.UniformSpread(FRINESI_BASE_SPREAD_PUMP)
	.BulletsPerShot(FRINESI_PELLETS_PER_SHOT)
	.DamagePerShot(FRINESI_BASE_DAMAGE_PUMP)
	.AutoAim(FRINESI_AUTOAIM_DEG)
	.ShellModelName("models/shell.mdl") // TODO: Nightfire has a shell model - use that? Multiple skins?
	.AnimIndex_FireNotEmpty(FRINESI_SHOOT_BIG)
	.ViewPunchY(FRINESI_RECOIL_PUMP)
	.Volume(LOUD_GUN_VOLUME)
	.MuzzleFlashBrightness(NORMAL_GUN_FLASH)
	.Sounds(CGenericWeaponAttributes_Sound()
		.Sound("weapons/weapon_frinesi/frinesi_altfire.wav")
		.MinVolume(0.95f)
		.MaxVolume(1.0f)
		.MinPitch(98)
		.MaxPitch(102)
	)
))
.Animations(
	CGenericWeaponAtts_Animations()
	.Extension("shotgun")
	.Index_Draw(FRINESI_DRAW)
	.Index_ReloadWhenNotEmpty(FRINESI_START_RELOAD)
);

LINK_ENTITY_TO_CLASS(weapon_frinesi, CWeaponFrinesi)

CWeaponFrinesi::CWeaponFrinesi()
	: CGenericWeapon(),
	  m_flReloadStartDuration(0.0f),
	  m_flReloadDuration(0.0f),
	  m_flPumpDuration(0.0f)
{
}

void CWeaponFrinesi::Precache()
{
	CGenericWeapon::Precache();

	// Cache the durations for our reload animations, so we can use them later.
	m_flReloadStartDuration = ViewModelAnimationDuration(FRINESI_START_RELOAD);
	m_flReloadDuration = ViewModelAnimationDuration(FRINESI_RELOAD);
	m_flPumpDuration = ViewModelAnimationDuration(FRINESI_PUMP);
}

void CWeaponFrinesi::PrimaryAttack()
{
	if ( FlagReloadInterrupt() )
	{
		return;
	}

	CGenericWeapon::PrimaryAttack();
}

void CWeaponFrinesi::SecondaryAttack()
{
	if ( FlagReloadInterrupt() )
	{
		WeaponIdle();
	}

	CGenericWeapon::SecondaryAttack();
}

void CWeaponFrinesi::Holster(int skipLocal)
{
	CGenericWeapon::Holster(skipLocal);
	DelayPendingActions(0.1f, true);
}

void CWeaponFrinesi::WeaponTick()
{
	// If we're reloading and have already flagged an interrupt, clear the input buttons.
	// This is to prevent the player holding down attack buttons and preventing
	// the weapon going into idle state.
	if ( (m_fInSpecialReload & RELOAD_MASK) && (m_fInSpecialReload & RELOAD_FLAG_INTERRUPTED) )
	{
		m_pPlayer->pev->button &= ~(IN_ATTACK | IN_ATTACK2);
	}
}

bool CWeaponFrinesi::FlagReloadInterrupt()
{
	// If we're reloading, allow an interruption.
	if ( (m_fInSpecialReload & RELOAD_MASK) != RELOAD_IDLE )
	{
		m_fInSpecialReload |= RELOAD_FLAG_INTERRUPTED;
		return true;
	}

	return false;
};

int CWeaponFrinesi::HandleSpecialReload(int currentState)
{
	switch (currentState & RELOAD_MASK)
	{
		case RELOAD_IDLE:
		{
			SendWeaponAnim(FRINESI_START_RELOAD);

			// Set both our next firing times to be now.
			// This allows the player to flag a reload interrupt by firing
			// at any point from now on. It'll only be honoured at the
			// appropriate time, though.
			DelayFiring(0, true);

			// Go into load shell state after intro animation has finished.
			SetNextIdleTime(m_flReloadStartDuration, true);
			return NextReloadState(currentState, RELOAD_LOAD_SHELL);
		}

		case RELOAD_LOAD_SHELL:
		{
			// If we haven't finished whatever animation is currently playing, don't change state.
			if( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
			{
				return NextReloadState(currentState, RELOAD_LOAD_SHELL);
			}

			if ( ((currentState & RELOAD_FLAG_INTERRUPTED) && (currentState & RELOAD_FLAG_LOADED_ONCE)) ||
				 m_iClip >= WeaponAttributes().Core().MaxClip() ||
				 m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1 )
			{
				// Reloading has finished. Do a pump and delay any further activity until it's finished.
				SendWeaponAnim(FRINESI_PUMP);
				DelayPendingActions(m_flPumpDuration, true);

				return NextReloadState(0, RELOAD_IDLE);
			}

			// TODO: Make these come from the MDL?
			if( RANDOM_LONG( 0, 1 ) )
			{
				EMIT_SOUND_DYN( ENT( m_pPlayer->pev ), CHAN_ITEM, "weapons/reload1.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG( 0, 0x1f ) );
			}
			else
			{
				EMIT_SOUND_DYN( ENT( m_pPlayer->pev ), CHAN_ITEM, "weapons/reload3.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG( 0, 0x1f ) );
			}

			SendWeaponAnim(FRINESI_RELOAD);

			// Go into the increment clip state once this animation has finished.
			SetNextIdleTime(m_flReloadDuration, true);
			return NextReloadState(currentState, RELOAD_INCREMENT_CLIP);
		}

		case RELOAD_INCREMENT_CLIP:
		{
			if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] > 0 )
			{
				++m_iClip;
				--m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType];
			}

			return NextReloadState(currentState, RELOAD_LOAD_SHELL | RELOAD_FLAG_LOADED_ONCE);
		}

		default:
		{
			return NextReloadState(0, RELOAD_IDLE);
		}
	};
}

const CGenericWeaponAttributes& CWeaponFrinesi::WeaponAttributes() const
{
	return StaticWeaponAttributes;
}
