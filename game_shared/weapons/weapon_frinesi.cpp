#include "weapon_frinesi.h"

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

#ifdef CLIENT_DLL
static CWeaponFrinesi PredictionWeapon;
#endif

// From Nightfire:
// - 6 bullets per shot in either mode
// - 81 damage over all 6 shots in auto mode = 13.5 per shot
// - 159 damage over all 6 shots in pump mode = 26.5 per shot


namespace
{
	constexpr uint8_t FRINESI_PELLETS_PER_SHOT = 6;
	constexpr float FRINESI_BASE_DAMAGE_AUTO = 81.0f / static_cast<float>(FRINESI_PELLETS_PER_SHOT);
	constexpr float FRINESI_BASE_DAMAGE_PUMP = 159.0f / static_cast<float>(FRINESI_PELLETS_PER_SHOT);
	constexpr float FRINESI_BASE_SPREAD_AUTO = 0.05f;
	constexpr float FRINESI_BASE_SPREAD_PUMP = 0.1f;
	constexpr float FRINESI_AUTOAIM_DEG = AUTOAIM_5DEGREES;
	constexpr float FRINESI_FIRE_RATE_AUTO = 4.0f;
	constexpr float FRINESI_FIRE_RATE_PUMP = 1.0f;
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
	.ViewPunchY(-3.0f)
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
	.ViewPunchY(-4.0f)
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

const CGenericWeaponAttributes& CWeaponFrinesi::WeaponAttributes() const
{
	return StaticWeaponAttributes;
}
