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

int CWeaponFrinesi::HandleSpecialReload(int currentState)
{
	switch (currentState)
	{
		case RELOAD_IDLE:
		{
			SendWeaponAnim(FRINESI_START_RELOAD);

			// The next time we process idle tasks is after this "preroll" animation.
			SetNextIdleTime(m_flReloadStartDuration, true);

			// Not exactly sure what the player's next attack variable is used for - to postpone
			// usage of any weapon at all? The HL shotgun set it, so we do too.
			m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + m_flReloadStartDuration;

			// The next time any firing is allowed is this animation, plus a shell reload and a pump.
			const float nextFire = m_flReloadStartDuration + m_flReloadDuration + m_flPumpDuration;
			DelayFiring(nextFire);

			return RELOAD_LOAD_SHELL;
		}

		case RELOAD_LOAD_SHELL:
		{
			// If we haven't finished the animation yet, don't change state.
			if( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
			{
				return RELOAD_LOAD_SHELL;
			}

			if ( m_iClip >= WeaponAttributes().Core().MaxClip() || m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < 1 )
			{
				// Can't reload any more.
				SendWeaponAnim(FRINESI_PUMP);
				DelayPendingActions(m_flPumpDuration);

				return RELOAD_IDLE;
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
			DelayPendingActions(m_flReloadDuration);

			return RELOAD_INCREMENT_CLIP;
		}

		case RELOAD_INCREMENT_CLIP:
		{
			m_iClip += 1;
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1;
			return RELOAD_LOAD_SHELL;
		}

		default:
		{
			return RELOAD_IDLE;
		}
	};
}

const CGenericWeaponAttributes& CWeaponFrinesi::WeaponAttributes() const
{
	return StaticWeaponAttributes;
}
