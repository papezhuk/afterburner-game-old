#include "weapon_p99.h"
#include "weaponregistry.h"
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
	enum P99Animations_e
	{
		P99_IDLE1 = 0,
		P99_IDLE2,
		P99_IDLE3,
		P99_SHOOT,
		P99_SHOOT_EMPTY,
		P99_RELOAD,
		P99_RELOAD_NOT_EMPTY,
		P99_DRAW,
		P99_HOLSTER,
		P99_ADD_SILENCER,
		P99_REMOVE_SILENCER,
		P99_SHOOT_SILENCER,
		P99_SHOOT_SILENCER_EMPTY,
	};

	enum P99Body_e
	{
		P99BODY_UNSILENCED = 0,
		P99BODY_SILENCED
	};

	static constexpr float P99_FIRE_RATE = 6.0f;
	static constexpr float P99_BOT_REFIRE_DELAY = 1.0f/6.0f;
	static constexpr float P99_AUTOAIM_DEG = AUTOAIM_10DEGREES;
	static constexpr float P99_BASE_DAMAGE = 20.0f;
	static constexpr float P99_BASE_SPREAD = 0.03f;
	static constexpr int P99_AMMOBOX_GIVE = 32;

#ifdef CLIENT_DLL
	static CWeaponP99 PredictionWeapon;
#else
	float P99DesireToUse(CGenericWeapon& weapon, CBaseBot&, CBaseEntity&, float distanceToEnemy)
	{
		return static_cast<float>(WeaponPref_P99) / static_cast<float>(WeaponPref_Max);
	}

	void P99UseWeapon(CGenericWeapon& weapon, CBaseBotFightStyle& fightStyle)
	{
		fightStyle.SetSecondaryFire(false);
		fightStyle.RandomizeAimAtHead(80);
		fightStyle.SetNextShootTime(1.0f / P99_FIRE_RATE, P99_BOT_REFIRE_DELAY, 0.4f, 0.7f);
	}
#endif
}

static const CGenericWeaponAttributes StaticWeaponAttributes = CGenericWeaponAttributes(
	CGenericWeaponAtts_Core()
	.Id(WeaponId_e::WeaponP99)
	.Classname("weapon_p99")
	.Flags(0)
	.SwitchWeight(WeaponPref_P99)
	.PrimaryAmmoDef(&AmmoDef_P99)
	.PrimaryAmmoClassname("ammo_p99")
	.MaxClip(16)
	.PrimaryAmmoOnFirstPickup(16)
	.ViewModelName("models/weapon_p99/v_p99.mdl")
	.PlayerModelName("models/weapon_p99/p_p99.mdl")
	.WorldModelName("models/weapon_p99/w_p99.mdl")
#ifdef CLIENT_DLL
	.ClientPredictionWeapon(&PredictionWeapon)
#endif
)
// Unsilenced
.FireMode(0, CGenericWeaponAtts_FireMode()
	.Event("events/weapon_p99/fire01.sc")
	.FiresUnderwater(true)
	.UsesAmmo(CGenericWeaponAtts_FireMode::AmmoType_e::Primary)
	.FireRate(P99_FIRE_RATE)
	.UniformSpread(P99_BASE_SPREAD)
	.ViewModelBodyOverride(P99BODY_UNSILENCED)
	.AnimIndex_FireNotEmpty(P99_SHOOT)
	.AnimIndex_FireEmpty(P99_SHOOT_EMPTY)
	.ViewPunchY(-2.0f)
	.Volume(NORMAL_GUN_VOLUME)
	.MuzzleFlashBrightness(NORMAL_GUN_FLASH)
	.Sounds(CGenericWeaponAttributes_Sound()
		.Sound("weapons/weapon_p99/p99_fire1.wav")
		.MinVolume(0.92f)
		.MaxVolume(1.0f)
		.MinPitch(98)
		.MaxPitch(101)
	)
	.Mechanic(&((*new CGenericWeaponAtts_HitscanFireMechanic())
		.BaseDamagePerShot(&skilldata_t::plrDmgP99)
		.AutoAim(P99_AUTOAIM_DEG)
		.ShellModelName("models/shell.mdl") // TODO: Does Nightfire have shell models? Could we make some?
	))
)
// Silenced
.FireMode(1, CGenericWeaponAtts_FireMode()
	.Event("events/weapon_p99/fire02.sc")
	.FiresUnderwater(true)
	.UsesAmmo(CGenericWeaponAtts_FireMode::AmmoType_e::Primary)
	.FireRate(P99_FIRE_RATE)
	.UniformSpread(P99_BASE_SPREAD)
	.ViewModelBodyOverride(P99BODY_SILENCED)
	.AnimIndex_FireNotEmpty(P99_SHOOT)
	.AnimIndex_FireEmpty(P99_SHOOT_EMPTY)
	.ViewPunchY(-2.0f)
	.Volume(QUIET_GUN_VOLUME)
	.MuzzleFlashBrightness(DIM_GUN_FLASH)
	.Sounds(CGenericWeaponAttributes_Sound()
		.Sound("weapons/weapon_p99/p99_fire_sil1.wav")
		.MinVolume(0.92f)
		.MaxVolume(1.0f)
		.MinPitch(98)
		.MaxPitch(101)
	)
	.Mechanic(&((*new CGenericWeaponAtts_HitscanFireMechanic())
		.BaseDamagePerShot(&skilldata_t::plrDmgP99)
		.AutoAim(P99_AUTOAIM_DEG)
		.ShellModelName("models/shell.mdl") // TODO: Does Nightfire have shell models? Could we make some?
	))
)
.Animations(
	CGenericWeaponAtts_Animations()
	.Extension("onehanded")
	.Index_Draw(P99_DRAW)
	.Index_ReloadWhenEmpty(P99_RELOAD)
	.Index_ReloadWhenNotEmpty(P99_RELOAD_NOT_EMPTY)
)
.IdleAnimations(
	CGenericWeaponAtts_IdleAnimations()
	.Animation(P99_IDLE1)
	.Animation(P99_IDLE2)
	.Animation(P99_IDLE3)
)
.Skill(
	CGenericWeaponAttributes_Skill()
	.Record("sk_plr_dmg_p99", &skilldata_t::plrDmgP99)
)
#ifndef CLIENT_DLL
.BotWeaponAttributes(
	CBotWeaponAttributes()
	.DesireToUse(&P99DesireToUse)
	.UseWeapon(&P99UseWeapon)
)
#endif
;

LINK_ENTITY_TO_CLASS(weapon_p99, CWeaponP99)
LINK_ENTITY_TO_CLASS(weapon_pp9, CWeaponP99)	// NF devs called it "PP9" for some reason.

#ifdef AFTERBURNER_GAMEPLAY_PLACEHOLDERS
// To make weapons less sparse for testing, map some other known weapons to this one.
LINK_ENTITY_TO_CLASS(weapon_kowloon, CWeaponP99)
LINK_ENTITY_TO_CLASS(weapon_mp9, CWeaponP99)
#endif

CWeaponP99::CWeaponP99()
	: CGenericHitscanWeapon(),
	  m_bSilenced(false)
{
}

const CGenericWeaponAttributes& CWeaponP99::WeaponAttributes() const
{
	return StaticWeaponAttributes;
}

void CWeaponP99::PrimaryAttack()
{
	FireUsingMode(m_bSilenced ? 1 : 0);
}

void CWeaponP99::SecondaryAttack()
{
	const int anim = m_bSilenced ? P99_REMOVE_SILENCER : P99_ADD_SILENCER;

	// We must animate using the silenced body group in either case, so we can see the silencer.
	SendWeaponAnim(anim, P99BODY_SILENCED);

	m_bSilenced = !m_bSilenced;
	SetViewModelBody(m_bSilenced ? P99BODY_SILENCED : P99BODY_UNSILENCED);

	DelayPendingActions(ViewModelAnimationDuration(anim));
}

bool CWeaponP99::ReadPredictionData(const weapon_data_t* from)
{
	if ( !CGenericWeapon::ReadPredictionData(from) )
	{
		return false;
	}

	const bool newSilencedState = from->iuser1 == 1;
	if ( m_bSilenced != newSilencedState )
	{
		m_bSilenced = newSilencedState;

		// The silenced state has changed, so immediately update the viewmodel body to be accurate.
		SetViewModelBody(m_bSilenced ? P99BODY_SILENCED : P99BODY_UNSILENCED, true);
	}

	return true;
}

bool CWeaponP99::WritePredictionData(weapon_data_t* to)
{
	if ( !CGenericWeapon::WritePredictionData(to) )
	{
		return false;
	}

	to->iuser1 = m_bSilenced ? 1 : 0;
	return true;
}

#ifndef CLIENT_DLL
TYPEDESCRIPTION	CWeaponP99::m_SaveData[] =
{
	DEFINE_FIELD(CWeaponP99, m_bSilenced, FIELD_BOOLEAN)
};

IMPLEMENT_SAVERESTORE(CWeaponP99, CGenericWeapon)
#endif

class CAmmoP99 : public CGenericAmmo
{
public:
	CAmmoP99()
		: CGenericAmmo("models/weapon_p99/w_ammo_p99.mdl", AmmoDef_P99, P99_AMMOBOX_GIVE)
	{
	}
};

LINK_ENTITY_TO_CLASS(ammo_p99, CAmmoP99)

#ifdef AFTERBURNER_GAMEPLAY_PLACEHOLDERS
// To make weapons less sparse for testing, map some other known ammo to this one.
LINK_ENTITY_TO_CLASS(ammo_kowloon, CAmmoP99)
LINK_ENTITY_TO_CLASS(ammo_mp9, CAmmoP99)
#endif