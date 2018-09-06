#include "weapon_p99.h"
#include "weaponregistry.h"

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

#ifdef CLIENT_DLL
static CWeaponP99 PredictionWeapon;
#endif

namespace
{
	// For consistency between fire modes:
	constexpr float P99_FIRE_RATE = 6.0f;
	constexpr float P99_AUTOAIM_DEG = AUTOAIM_10DEGREES;
	constexpr float P99_BASE_DAMAGE = 10.0f;	// TODO: Tweak after testing
	constexpr float P99_BASE_SPREAD = 0.05f;	// TODO: Tweak after testing
}

static const CGenericWeaponAttributes StaticWeaponAttributes = CGenericWeaponAttributes(
	CGenericWeaponAtts_Core()
	.Id(WeaponId_e::WeaponP99)
	.Classname("weapon_p99")
	.WeaponSlot(1)			// TODO: Remove these from attributes, define slots elsewhere
	.WeaponSlotPosition(3)
	.Flags(0)
	.SwitchWeight(0)
	.PrimaryAmmoDef(&AmmoDef_P99)
	.MaxClip(16)
	.PrimaryAmmoOnFirstPickup(48)
	.ViewModelName("models/v_p99.mdl")
	.PlayerModelName("models/p_p99.mdl")
	.WorldModelName("models/w_p99.mdl")
#ifdef CLIENT_DLL
	.ClientPredictionWeapon(&PredictionWeapon)
#endif
)
.FireMode(0,
	&((*new CGenericWeaponAtts_HitscanFireMode("events/weapon_p99_fire01.sc"))
	.FireRate(P99_FIRE_RATE)
	.UniformSpread(P99_BASE_SPREAD)
	.DamagePerShot(P99_BASE_DAMAGE)
	.AutoAim(P99_AUTOAIM_DEG)
	.ShellModelName("models/shell.mdl") // TODO: Does Nightfire have shell models? Could we make some?
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
	&((*new CGenericWeaponAtts_HitscanFireMode("events/weapon_p99_fire02.sc"))
	.FireRate(P99_FIRE_RATE)
	.UniformSpread(P99_BASE_SPREAD)
	.DamagePerShot(P99_BASE_DAMAGE)
	.AutoAim(P99_AUTOAIM_DEG)
	.ShellModelName("models/shell.mdl") // TODO: Does Nightfire have shell models? Could we make some?
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

LINK_ENTITY_TO_CLASS( weapon_p99, CWeaponP99 )

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
	if ( m_bSilenced )
	{
		SendWeaponAnim(P99_REMOVE_SILENCER, pev->body);
		pev->body = 0;
	}
	else
	{
		pev->body = 1;
		SendWeaponAnim(P99_ADD_SILENCER, pev->body);
	}

	m_bSilenced = !m_bSilenced;

	// TODO: Increment this by the duration of the actual animation
	float next = UTIL_WeaponTimeBase() + 2;
	m_flNextSecondaryAttack = next;
	m_flNextSecondaryAttack = next;
	m_flTimeWeaponIdle = next;
}
