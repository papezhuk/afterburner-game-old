#include "weapon_p99.h"
#include "weaponregistry.h"
#include "weaponinfo.h"

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

#ifdef CLIENT_DLL
static CWeaponP99 PredictionWeapon;
#endif

namespace
{
	// For consistency between fire modes:
	constexpr float P99_FIRE_RATE = 6.0f;
	constexpr float P99_AUTOAIM_DEG = AUTOAIM_10DEGREES;
	constexpr float P99_BASE_DAMAGE = 20.0f;
	constexpr float P99_BASE_SPREAD = 0.03f;
}

static const CGenericWeaponAttributes StaticWeaponAttributes = CGenericWeaponAttributes(
	CGenericWeaponAtts_Core()
	.Id(WeaponId_e::WeaponP99)
	.Classname("weapon_p99")
	.Flags(0)
	.SwitchWeight(0)
	.PrimaryAmmoDef(&AmmoDef_P99)
	.MaxClip(16)
	.PrimaryAmmoOnFirstPickup(16)
	.ViewModelName("models/weapon_p99/v_p99.mdl")
	.PlayerModelName("models/weapon_p99/p_p99.mdl")
	.WorldModelName("models/weapon_p99/w_p99.mdl")
#ifdef CLIENT_DLL
	.ClientPredictionWeapon(&PredictionWeapon)
#endif
)
.FireMode(0,
	&((*new CGenericWeaponAtts_HitscanFireMode("events/weapon_p99/fire01.sc"))
	.FireRate(P99_FIRE_RATE)
	.UniformSpread(P99_BASE_SPREAD)
	.DamagePerShot(P99_BASE_DAMAGE)
	.AutoAim(P99_AUTOAIM_DEG)
	.ShellModelName("models/shell.mdl") // TODO: Does Nightfire have shell models? Could we make some?
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
))
.FireMode(1,
	&((*new CGenericWeaponAtts_HitscanFireMode("events/weapon_p99/fire02.sc"))
	.FireRate(P99_FIRE_RATE)
	.UniformSpread(P99_BASE_SPREAD)
	.DamagePerShot(P99_BASE_DAMAGE)
	.AutoAim(P99_AUTOAIM_DEG)
	.ShellModelName("models/shell.mdl") // TODO: Does Nightfire have shell models? Could we make some?
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
))
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
);

LINK_ENTITY_TO_CLASS(weapon_p99, CWeaponP99)
LINK_ENTITY_TO_CLASS(weapon_pp9, CWeaponP99)	// NF devs called it "PP9" for some reason.

CWeaponP99::CWeaponP99()
	: CGenericWeapon(),
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
