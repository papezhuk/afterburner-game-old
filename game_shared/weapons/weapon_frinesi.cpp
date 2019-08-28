#include "weapon_frinesi.h"
#include "weaponinfo.h"
#include "skill.h"
#include "gamerules.h"
#include "weapon_pref_weights.h"
#include "weaponatts_hitscanfiremechanic.h"

#ifndef CLIENT_DLL
#include "bot.h"
#endif

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

	static constexpr int RELOAD_MASK = 0x3;
	static constexpr int RELOAD_FLAG_INTERRUPTED = (1 << 2);
	static constexpr int RELOAD_FLAG_LOADED_ONCE = (1 << 3);
	static constexpr int NextReloadState(int orig, int next)
	{
		return (orig & ~(RELOAD_MASK)) | next;
	}

	// From Nightfire:
	// - 6 bullets per shot in either mode
	// - 81 damage over all 6 shots in auto mode = 13.5 per shot
	// - 159 damage over all 6 shots in pump mode = 26.5 per shot

	static constexpr uint8_t FRINESI_PELLETS_PER_SHOT = 6;
	static constexpr float FRINESI_AUTOAIM_DEG = AUTOAIM_5DEGREES;
	static constexpr int FRINESI_AMMOBOX_GIVE = 20;

	static constexpr float FRINESI_BASE_DAMAGE_AUTO = 81.0f / static_cast<float>(FRINESI_PELLETS_PER_SHOT);
	static constexpr float FRINESI_BASE_SPREAD_AUTO = 0.05f;
	static constexpr float FRINESI_FIRE_RATE_AUTO = 4.0f;
	static constexpr float FRINESI_RECOIL_AUTO = -5.0f;

	// Go from 4 shots/sec to 3 shots/sec
	static constexpr float FRINESI_BOT_REFIRE_DELAY_AUTO = (1.0f/3.0f) - (1.0f/FRINESI_FIRE_RATE_AUTO);

	static constexpr float FRINESI_BASE_DAMAGE_PUMP = 159.0f / static_cast<float>(FRINESI_PELLETS_PER_SHOT);
	static constexpr float FRINESI_BASE_SPREAD_PUMP = 0.1f;
	static constexpr float FRINESI_FIRE_RATE_PUMP = 1.0f;
	static constexpr float FRINESI_RECOIL_PUMP = -10.0f;
	static constexpr float FRINESI_PUMP_DELAY = 0.42f;

	// Go from 1 shot/sec to 1 shot per 1.5 secs
	static constexpr float FRINESI_BOT_REFIRE_DELAY_PUMP = 1.5f - (1.0f/FRINESI_FIRE_RATE_PUMP);

#ifdef CLIENT_DLL
	static CWeaponFrinesi PredictionWeapon;
#else
	float FrinesiDesireToUse(CGenericWeapon& weapon, CBaseBot&, CBaseEntity&, float distanceToEnemy)
	{
		return static_cast<float>(WeaponPref_Frinesi) / static_cast<float>(WeaponPref_Max);
	}

	void FrinesiUseWeapon(CGenericWeapon& weapon, CBaseBotFightStyle& fightStyle)
	{
		static constexpr float SECONDARY_FIRE_PROXIMITY = 700.0f;

		fightStyle.RandomizeAimAtHead(60);
		int chanceOfSecondaryFire = 20;

		// If we're able to determine that the enemy is near enough,
		// increase the chance of using secondary fire.

		CBaseBot* bot = fightStyle.GetOwner();
		CBaseEntity* enemy = bot->GetEnemy();

		if ( enemy && (enemy->pev->origin - bot->pev->origin).Length() <= SECONDARY_FIRE_PROXIMITY )
		{
			chanceOfSecondaryFire = 90;
		}

		fightStyle.RandomizeSecondaryFire(chanceOfSecondaryFire);
		fightStyle.SetNextShootTime(1.0f / (fightStyle.GetSecondaryFire() ? FRINESI_FIRE_RATE_PUMP : FRINESI_FIRE_RATE_AUTO),
									fightStyle.GetSecondaryFire() ? FRINESI_BOT_REFIRE_DELAY_PUMP : FRINESI_BOT_REFIRE_DELAY_AUTO,
									0.8f, 2.0f);
	}
#endif
}

static const CGenericWeaponAttributes StaticWeaponAttributes = CGenericWeaponAttributes(
	CGenericWeaponAtts_Core()
	.Id(WeaponId_e::WeaponFrinesi)
	.Classname("weapon_frinesi")
	.Flags(0)
	.SwitchWeight(WeaponPref_Frinesi)
	.PrimaryAmmoDef(&AmmoDef_Frinesi)
	.PrimaryAmmoClassname("ammo_frinesi")
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
// Auto
.FireMode(0, CGenericWeaponAtts_FireMode()
	.Event("events/weapon_frinesi/fire01.sc")
	.FiresUnderwater(true)
	.UsesAmmo(CGenericWeaponAtts_FireMode::AmmoType_e::Primary)
	.FireRate(FRINESI_FIRE_RATE_AUTO)
	.UniformSpread(FRINESI_BASE_SPREAD_AUTO)
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
	.Mechanic(&((*new CGenericWeaponAtts_HitscanFireMechanic())
		.BulletsPerShot(FRINESI_PELLETS_PER_SHOT)
		.BaseDamagePerShot(&skilldata_t::plrDmgFrinesiAuto)
		.AutoAim(FRINESI_AUTOAIM_DEG)
		.ShellModelName("models/shell.mdl") // TODO: Nightfire has a shell model - use that? Multiple skins?
	))
)
// Pump
.FireMode(1, CGenericWeaponAtts_FireMode()
	.Event("events/weapon_frinesi/fire02.sc")
	.FiresUnderwater(true)
	.UsesAmmo(CGenericWeaponAtts_FireMode::AmmoType_e::Primary)
	.FireRate(FRINESI_FIRE_RATE_PUMP)
	.UniformSpread(FRINESI_BASE_SPREAD_PUMP)
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
	.Mechanic(&((*new CGenericWeaponAtts_HitscanFireMechanic())
		.BulletsPerShot(FRINESI_PELLETS_PER_SHOT)
		.BaseDamagePerShot(&skilldata_t::plrDmgFrinesiPump)
		.AutoAim(FRINESI_AUTOAIM_DEG)
		.ShellModelName("models/shell.mdl") // TODO: Nightfire has a shell model - use that? Multiple skins?
	))
)
.Animations(
	CGenericWeaponAtts_Animations()
	.Extension("shotgun")
	.Index_Draw(FRINESI_DRAW)
	.Index_ReloadWhenNotEmpty(FRINESI_START_RELOAD)
	.ReloadSounds(CGenericWeaponAttributes_Sound()
		.Sound("weapons/weapon_frinesi/frinesi_reload1.wav")
		.Sound("weapons/weapon_frinesi/frinesi_reload2.wav")
		.MinVolume(1.0f)
		.MaxVolume(1.0f)
		.MinPitch(98)
		.MaxPitch(102)
	)
)
.Skill(
	CGenericWeaponAttributes_Skill()
	.Record("sk_plr_dmg_frinesi_auto", &skilldata_t::plrDmgFrinesiAuto)
	.Record("sk_plr_dmg_frinesi_pump", &skilldata_t::plrDmgFrinesiPump)
)
#ifndef CLIENT_DLL
.BotWeaponAttributes(
	CBotWeaponAttributes()
	.DesireToUse(&FrinesiDesireToUse)
	.UseWeapon(&FrinesiUseWeapon)
)
#endif
;

LINK_ENTITY_TO_CLASS(weapon_frinesi, CWeaponFrinesi)

#ifdef AFTERBURNER_GAMEPLAY_PLACEHOLDERS
// To make weapons less sparse for testing, map some other known weapons to this one.
LINK_ENTITY_TO_CLASS(weapon_commando, CWeaponFrinesi)
LINK_ENTITY_TO_CLASS(weapon_minigun, CWeaponFrinesi)
#endif

CWeaponFrinesi::CWeaponFrinesi()
	: CGenericHitscanWeapon(),
	  m_flReloadStartDuration(0.0f),
	  m_flReloadDuration(0.0f),
	  m_flPumpDuration(0.0f),
	  m_flNextPumpTime(0.0f)
{
}

void CWeaponFrinesi::Precache()
{
	CGenericWeapon::Precache();
	PRECACHE_SOUND("weapons/weapon_frinesi/frinesi_cock.wav");

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

	FireUsingMode(0);
}

void CWeaponFrinesi::SecondaryAttack()
{
	if ( FlagReloadInterrupt() )
	{
		return;
	}

	if ( FireUsingMode(1) )
	{
		m_flNextPumpTime = gpGlobals->time + FRINESI_PUMP_DELAY;
	}
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
	// the weapon going into an idle state.
	if ( (m_fInSpecialReload & RELOAD_MASK) && (m_fInSpecialReload & RELOAD_FLAG_INTERRUPTED) )
	{
		m_pPlayer->pev->button &= ~(IN_ATTACK | IN_ATTACK2);
	}

	if ( m_flNextPumpTime != 0.0f && m_flNextPumpTime < gpGlobals->time )
	{
		PlayPumpSound();
		m_flNextPumpTime = 0.0f;
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
			m_flNextPumpTime = 0.0f;
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
				m_flNextPumpTime = gpGlobals->time + 0.1f;
				SendWeaponAnim(FRINESI_PUMP);
				DelayPendingActions(m_flPumpDuration, true);

				return NextReloadState(0, RELOAD_IDLE);
			}

			PlaySound(WeaponAttributes().Animations().ReloadSounds(), CHAN_ITEM);
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

void CWeaponFrinesi::PlayPumpSound()
{
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev),
					   CHAN_ITEM,
					   "weapons/weapon_frinesi/frinesi_cock.wav",
					   1.0f,
					   ATTN_NORM,
					   0,
					   UTIL_SharedRandomLong(m_pPlayer->random_seed, 98, 101));
}

bool CWeaponFrinesi::ReadPredictionData(const weapon_data_t* from)
{
	if ( !CGenericWeapon::ReadPredictionData(from) )
	{
		return false;
	}

	m_flNextPumpTime = from->fuser1;
	return true;
}

bool CWeaponFrinesi::WritePredictionData(weapon_data_t* to)
{
	if ( !CGenericWeapon::WritePredictionData(to) )
	{
		return false;
	}

	to->fuser1 = m_flNextPumpTime;
	return true;
}

const CGenericWeaponAttributes& CWeaponFrinesi::WeaponAttributes() const
{
	return StaticWeaponAttributes;
}

#ifndef CLIENT_DLL
TYPEDESCRIPTION	CWeaponFrinesi::m_SaveData[] =
{
	DEFINE_FIELD(CWeaponFrinesi, m_flNextPumpTime, FIELD_FLOAT)
};

IMPLEMENT_SAVERESTORE(CWeaponFrinesi, CGenericWeapon)
#endif

class CAmmoFrinesi : public CGenericAmmo
{
public:
	CAmmoFrinesi()
		: CGenericAmmo("models/weapon_frinesi/w_ammo_shotgun.mdl", AmmoDef_Frinesi, FRINESI_AMMOBOX_GIVE)
	{
	}
};

LINK_ENTITY_TO_CLASS(ammo_frinesi, CAmmoFrinesi)
LINK_ENTITY_TO_CLASS(ammo_shotgun, CAmmoFrinesi)	// For Nightfire compatibility

#ifdef AFTERBURNER_GAMEPLAY_PLACEHOLDERS
// To make weapons less sparse for testing, map some other known ammo to this one.
LINK_ENTITY_TO_CLASS(ammo_commando, CAmmoFrinesi)
LINK_ENTITY_TO_CLASS(ammo_mini, CAmmoFrinesi)
#endif